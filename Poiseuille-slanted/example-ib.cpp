// ---------------------------------------------------------------------
//
// Copyright (c) 2017 - 2023 by the IBAMR developers
// All rights reserved.
//
// This file is part of IBAMR.
//
// IBAMR is free software and is distributed under the 3-clause BSD
// license. The full text of the license can be found in the file
// COPYRIGHT at the top level directory of IBAMR.
//
// ---------------------------------------------------------------------
// Config files
#include <SAMRAI_config.h>

// Headers for basic PETSc functions
#include <petscsys.h>

// Headers for basic SAMRAI objects
#include <BergerRigoutsos.h>
#include <CartesianGridGeometry.h>
#include <LoadBalancer.h>
#include <StandardTagAndInitialize.h>

#include <ibamr/IBExplicitHierarchyIntegrator.h>
#include <ibamr/IBMethod.h>
#include <ibamr/IBStandardForceGen.h>
#include <ibamr/IBStandardInitializer.h>
#include <ibamr/INSCollocatedHierarchyIntegrator.h>
#include <ibamr/INSStaggeredHierarchyIntegrator.h>
#include <ibamr/StaggeredStokesOpenBoundaryStabilizer.h>

#include "ibtk/IndexUtilities.h"
#include <ibtk/AppInitializer.h>
#include <ibtk/IBTKInit.h>
#include <ibtk/IBTK_MPI.h>
#include <ibtk/LData.h>
#include <ibtk/LDataManager.h>
#include <ibtk/muParserCartGridFunction.h>
#include <ibtk/muParserRobinBcCoefs.h>

#include <ibamr/AdvDiffHierarchyIntegrator.h>
#include <ibtk/LData.h>
#include <ibtk/LDataManager.h>

#include <array>

#include <ibamr/app_namespaces.h>

// Application-specific includes
#include "PassiveTracers.h"

namespace ModelData
{
// Elasticity model data for thin body.

static double L = 0.0;
static double D = 0.0;
static double H = 0.0;
static double DX = 0.0;
static double MFAC = 0.0;

static double SPRING_CONSTANT = 0.0;
static double SPRING_DAMPING = 0.0;
static double dp = 0.0;
static double MU = 0.0;
static double THETA = 0.0;
static double yi = 0.0;
static double yo = 0.0;
static double U_MAX = 0.0;
static double x_loc_min, x_loc_max, y_loc_min, y_loc_max;
} // namespace ModelData
using namespace ModelData;

int finest_ln;
std::array<int, NDIM> N;
SAMRAI::tbox::Array<std::string> struct_list;
SAMRAI::tbox::Array<int> num_node;

static ofstream flow_rate_stream;
// Function prototypes
void compute_velocity_profile(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                              const int u_idx,
                              const double /*data_time*/,
                              const string& /*data_dump_dirname*/);

void compute_pressure_profile(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                              const int p_idx,
                              const double /*data_time*/,
                              const string& /*data_dump_dirname*/);

void output_data(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                 tbox::Pointer<IBHierarchyIntegrator> ins_integrator,
                 const int iteration_num,
                 const double loop_time,
                 const string& data_dump_dirname);

void compute_flow_rate(const tbox::Pointer<PatchHierarchy<NDIM> > hierarchy,
                       const int U_idx,
                       const double loop_time,
                       const int wgt_sc_idx);
void computeMarkerDisplacement(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy, Pointer<IBMethod> ib_method_ops);

void
generate_structure(const unsigned int& strct_num,
                   const int& ln,
                   int& num_vertices,
                   std::vector<IBTK::Point>& vertex_posn,
                   void* /*ctx*/)
{
    pout << "generate_structure for struct_num = " << strct_num << std::endl;
    pout << "struct_list[strct_num] = " << struct_list[strct_num] << std::endl;
    if (ln != finest_ln)
    {
        num_vertices = 0;
        vertex_posn.resize(num_vertices);
    }

    if (struct_list[strct_num].compare("pipe") == 0)
    {
        double ds = DX*MFAC;
        num_vertices = 2*(int(N[0]/MFAC)+1);
        pout << "num_vertices = " << num_vertices << std::endl;

        num_node[strct_num] = num_vertices;
        vertex_posn.resize(num_vertices);
        double CY1 = (L - L*tan(THETA) - D)/2 + D/2;
        double CY2 = CY1+L*tan(THETA);        
        for(int i=0; i< int(num_vertices/2); i++)
        {
            vertex_posn[i][0] = i*ds;
            vertex_posn[i][1] = CY1 + i*ds*tan(THETA) + D/2;
            vertex_posn[i+int(num_vertices/2)][0] = i*ds;
            vertex_posn[i+int(num_vertices/2)][1] = CY1 + i*ds*tan(THETA) - D/2;
        }
    }
    
    return;
}


void
generate_target_springs(
    const unsigned int& strct_num,
    const int& ln,
    std::multimap<int, IBRedundantInitializer::TargetSpec>& tg_pt_spec,
    void* /*ctx*/)
{
    if (ln != finest_ln) return;
    int num_vertices = 2*(int(N[0]/MFAC)+1);
    pout << "generate_target_springs for struct_num = " << strct_num << std::endl;

    if (struct_list[strct_num].compare("pipe") == 0)
    {
        for(int i=0; i< num_vertices; i++)
        {
            IBRedundantInitializer::TargetSpec tg;
            int force_fcn_idx = 0;
            tg.stiffness = SPRING_CONSTANT;
            tg.damping = SPRING_DAMPING;
            tg_pt_spec.insert(std::make_pair(i, tg));
        }
    }
    return;
}

/*******************************************************************************
 * For each run, the input filename and restart information (if needed) must   *
 * be given on the command line.  For non-restarted case, command line is:     *
 *                                                                             *
 *    executable <input file name>                                             *
 *                                                                             *
 * For restarted run, command line is:                                         *
 *                                                                             *
 *    executable <input file name> <restart directory> <restart number>        *
 *                                                                             *
 *******************************************************************************/
int
main(int argc, char* argv[])
{
    // Initialize IBAMR and libraries. Deinitialization is handled by this object as well.
    IBTKInit ibtk_init(argc, argv, MPI_COMM_WORLD);

    { // cleanup dynamically allocated objects prior to shutdown

        // Parse command line options, set some standard options from the input
        // file, initialize the restart database (if this is a restarted run),
        // and enable file logging.
        tbox::Pointer<AppInitializer> app_initializer = new AppInitializer(argc, argv, "IB.log");
        tbox::Pointer<tbox::Database> input_db = app_initializer->getInputDatabase();

        // Get various standard options set in the input file.
        const bool dump_viz_data = app_initializer->dumpVizData();
        const int viz_dump_interval = app_initializer->getVizDumpInterval();
        const bool uses_visit = dump_viz_data && app_initializer->getVisItDataWriter();

        const bool is_from_restart = app_initializer->isFromRestart();
        const bool dump_restart_data = app_initializer->dumpRestartData();
        const int restart_dump_interval = app_initializer->getRestartDumpInterval();
        const string restart_dump_dirname = app_initializer->getRestartDumpDirectory();

        const bool dump_postproc_data = app_initializer->dumpPostProcessingData();
        const int postproc_data_dump_interval = app_initializer->getPostProcessingDataDumpInterval();
        const string postproc_data_dump_dirname = app_initializer->getPostProcessingDataDumpDirectory();
        if (dump_postproc_data && (postproc_data_dump_interval > 0) && !postproc_data_dump_dirname.empty())
        {
            Utilities::recursiveMkdir(postproc_data_dump_dirname);
        }
        const bool dump_timer_data = app_initializer->dumpTimerData();
        const int timer_dump_interval = app_initializer->getTimerDumpInterval();

        // Create FE mesh.
        D = input_db->getDouble("D");              // channel height (cm)
        L = input_db->getDouble("L");              // channel length (cm)
        H = input_db->getDouble("H");              // channel length (cm)

        SPRING_CONSTANT = input_db->getDouble("SPRING_CONSTANT");
        SPRING_DAMPING = input_db->getDouble("SPRING_DAMPING");

        yo = input_db->getDouble("Yo");
        yi = input_db->getDouble("Yi");
        U_MAX = input_db->getDouble("U_MAX");
        DX = input_db->getDouble("DX");
        MFAC = input_db->getDouble("MFAC");

        MU = input_db->getDouble("MU");
        THETA = input_db->getDouble("THETA");
        dp = input_db->getDouble("DP");

        // Create major algorithm and data objects that comprise the
        // application.  These objects are configured from the input database
        // and, if this is a restarted run, from the restart database.
        tbox::Pointer<INSHierarchyIntegrator> navier_stokes_integrator = new INSStaggeredHierarchyIntegrator(
            "INSStaggeredHierarchyIntegrator",
            app_initializer->getComponentDatabase("INSStaggeredHierarchyIntegrator"));

        Pointer<IBMethod> ib_method_ops = new IBMethod("IBMethod", app_initializer->getComponentDatabase("IBMethod"));
        tbox::Pointer<IBHierarchyIntegrator> time_integrator =
            new IBExplicitHierarchyIntegrator("IBHierarchyIntegrator",
                                              app_initializer->getComponentDatabase("IBHierarchyIntegrator"),
                                              ib_method_ops,
                                              navier_stokes_integrator);
        tbox::Pointer<CartesianGridGeometry<NDIM> > grid_geometry = new CartesianGridGeometry<NDIM>(
            "CartesianGeometry", app_initializer->getComponentDatabase("CartesianGeometry"));
        tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy =
            new PatchHierarchy<NDIM>("PatchHierarchy", grid_geometry);
        tbox::Pointer<StandardTagAndInitialize<NDIM> > error_detector =
            new StandardTagAndInitialize<NDIM>("StandardTagAndInitialize",
                                               time_integrator,
                                               app_initializer->getComponentDatabase("StandardTagAndInitialize"));
        tbox::Pointer<BergerRigoutsos<NDIM> > box_generator = new BergerRigoutsos<NDIM>();
        tbox::Pointer<LoadBalancer<NDIM> > load_balancer =
            new LoadBalancer<NDIM>("LoadBalancer", app_initializer->getComponentDatabase("LoadBalancer"));
        tbox::Pointer<GriddingAlgorithm<NDIM> > gridding_algorithm =
            new GriddingAlgorithm<NDIM>("GriddingAlgorithm",
                                        app_initializer->getComponentDatabase("GriddingAlgorithm"),
                                        error_detector,
                                        box_generator,
                                        load_balancer);

        // Configure the IB solver by IBStandardInitializer.
        // if(1==0)
        // {
        Pointer<IBStandardInitializer> ib_initializer = new IBStandardInitializer(
            "IBStandardInitializer", app_initializer->getComponentDatabase("IBStandardInitializer"));
        ib_method_ops->registerLInitStrategy(ib_initializer);
        Pointer<IBStandardForceGen> ib_force_fcn = new IBStandardForceGen();
        ib_method_ops->registerIBLagrangianForceFunction(ib_force_fcn);
        // }
        // else
        // {
        //Configure the IB solver IBRedundantInitializer.
        // Pointer<IBRedundantInitializer> ib_initializer = new IBRedundantInitializer(
        //     "IBRedundantInitializer", app_initializer->getComponentDatabase("IBRedundantInitializer"));
        // struct_list.resizeArray(input_db->getArraySize("STRUCTURE_LIST"));
        // struct_list = input_db->getStringArray("STRUCTURE_LIST");
        // std::vector<std::string> struct_list_vec(struct_list.getSize());
        // for (int i = 0; i < struct_list.size(); ++i) struct_list_vec[i] = struct_list[i];
        // num_node.resizeArray(struct_list.getSize());
        // N[0] = N[1] = input_db->getInteger("N");
        // finest_ln = input_db->getInteger("MAX_LEVELS") - 1;
        // ib_initializer->setStructureNamesOnLevel(finest_ln, struct_list_vec);
        // ib_initializer->registerInitStructureFunction(generate_structure);
        // ib_initializer->registerInitTargetPtFunction(generate_target_springs);
        // ib_method_ops->registerLInitStrategy(ib_initializer);
        // Pointer<IBStandardForceGen> ib_force_fcn = new IBStandardForceGen();
        // ib_method_ops->registerIBLagrangianForceFunction(ib_force_fcn);
        // }




        // Create Eulerian initial condition specification objects.
        if (input_db->keyExists("VelocityInitialConditions"))
        {
            Pointer<CartGridFunction> u_init = new muParserCartGridFunction(
                "u_init", app_initializer->getComponentDatabase("VelocityInitialConditions"), grid_geometry);
            navier_stokes_integrator->registerVelocityInitialConditions(u_init);
        }

        if (input_db->keyExists("PressureInitialConditions"))
        {
            Pointer<CartGridFunction> p_init = new muParserCartGridFunction(
                "p_init", app_initializer->getComponentDatabase("PressureInitialConditions"), grid_geometry);
            navier_stokes_integrator->registerPressureInitialConditions(p_init);
        }

        // Create Eulerian boundary condition specification objects.
        vector<RobinBcCoefStrategy<NDIM>*> u_bc_coefs(NDIM, nullptr);
        const IntVector<NDIM>& periodic_shift = grid_geometry->getPeriodicShift();
        if (periodic_shift.min() > 0)
        {
            for (unsigned int d = 0; d < NDIM; ++d)
            {
                u_bc_coefs[d] = nullptr;
            }
        }
        else
        {
            for (unsigned int d = 0; d < NDIM; ++d)
            {
                const std::string bc_coefs_name = "u_bc_coefs_" + std::to_string(d);

                const std::string bc_coefs_db_name = "VelocityBcCoefs_" + std::to_string(d);

                u_bc_coefs[d] = new muParserRobinBcCoefs(
                    bc_coefs_name, app_initializer->getComponentDatabase(bc_coefs_db_name), grid_geometry);
            }
            navier_stokes_integrator->registerPhysicalBoundaryConditions(u_bc_coefs);
        }

        // Create Eulerian body force function specification objects.
        if (input_db->keyExists("ForcingFunction"))
        {
            Pointer<CartGridFunction> f_fcn = new muParserCartGridFunction(
                "f_fcn", app_initializer->getComponentDatabase("ForcingFunction"), grid_geometry);
            time_integrator->registerBodyForceFunction(f_fcn);
        }

        // Set up visualization plot file writers.
        tbox::Pointer<VisItDataWriter<NDIM> > visit_data_writer = app_initializer->getVisItDataWriter();
        Pointer<LSiloDataWriter> silo_data_writer = app_initializer->getLSiloDataWriter();
        if (uses_visit)
        {
            time_integrator->registerVisItDataWriter(visit_data_writer);
            ib_initializer->registerLSiloDataWriter(silo_data_writer);
            ib_method_ops->registerLSiloDataWriter(silo_data_writer);
        }

        // Initialize hierarchy configuration and data on all patches.
        time_integrator->initializePatchHierarchy(patch_hierarchy, gridding_algorithm);

        // Deallocate initialization objects.
        ib_method_ops->freeLInitStrategy();
        ib_initializer.setNull();
        app_initializer.setNull();

        // Set up locations to get velocity data.
        x_loc_min = input_db->getDoubleWithDefault("X_LOC_MIN", 0.1 * L);
        x_loc_max = input_db->getDoubleWithDefault("X_LOC_MAX", 0.9 * L);
        y_loc_min = input_db->getDoubleWithDefault("Y_LOC_MIN", 0.0);
        y_loc_max = input_db->getDoubleWithDefault("Y_LOC_MAX", L);

        // Print the input database contents to the log file.
        plog << "Input database:\n";
        input_db->printClassData(plog);

        // Write restart data before starting main time integration loop.
        if (dump_restart_data && !is_from_restart)
        {
            pout << "\nWriting restart files...\n\n";
            RestartManager::getManager()->writeRestartFile(restart_dump_dirname, 0);
        }

        // Write out initial visualization data.
        int iteration_num = time_integrator->getIntegratorStep();
        double loop_time = time_integrator->getIntegratorTime();
        if (dump_viz_data && uses_visit)
        {
            pout << "\n\nWriting visualization files...\n\n";
            time_integrator->setupPlotData();
            visit_data_writer->writePlotData(patch_hierarchy, iteration_num, loop_time);
            silo_data_writer->writePlotData(iteration_num, loop_time);
            // Pointer<IBExplicitHierarchyIntegrator> explicit_time_integrator = time_integrator;
            // explicit_time_integrator->setMarkers(tracer_positions);            
        }

        if (SAMRAI_MPI::getRank() == 0)
        {
            flow_rate_stream.open("flow_rate_outflow.curve",
                                  ios_base::out | ios_base::trunc);
            flow_rate_stream.precision(10);
        }
        // Main time step loop.
        double loop_time_end = time_integrator->getEndTime();
        double dt = 0.0;        
// #pragma region get Lagraingian points of the tracer structure
//     const int finest_ln = patch_hierarchy->getFinestLevelNumber();
//     IBTK::LDataManager* l_data_manager = ib_method_ops->getLDataManager();
//     const std::string structure_name = "pipe";
//     std::ofstream f_tracer("tracer.pt");

//     PassiveTracers tracers(structure_name, loop_time, f_tracer, finest_ln, l_data_manager);
//     std::vector<LagPoint> marker_position = tracers.getPositions();
//     Pointer<IBExplicitHierarchyIntegrator> explicit_time_integrator = time_integrator;
//     EigenAlignedVector<IBTK::Point> tracer_positions;
//     for (auto it = marker_position.begin(); it != marker_position.end(); ++it)
//     {
//         LagPoint& X = *it;
//         tracer_positions.emplace_back(X.x, X.y);
//     }                
//     explicit_time_integrator->setMarkers(tracer_positions); 
//     explicit_time_integrator->writeMarkerPlotData(iteration_num, loop_time);

// #pragma endregion

        while (!MathUtilities<double>::equalEps(loop_time, loop_time_end) && time_integrator->stepsRemaining())
        {
            iteration_num = time_integrator->getIntegratorStep();
            loop_time = time_integrator->getIntegratorTime();

            pout << "\n";
            pout << "+++++++++++++++++++++++++++++++++++++++++++++++++++\n";
            pout << "At beginning of timestep # " << iteration_num << "\n";
            pout << "Simulation time is " << loop_time << "\n";

            dt = time_integrator->getMaximumTimeStepSize();
            time_integrator->advanceHierarchy(dt);
            loop_time += dt;
/*
#pragma region get Lagraingian points of the tracer structure
    IBTK::LDataManager* l_data_manager = ib_method_ops->getLDataManager();
    const std::string structure_name = "pipe";
    PassiveTracers tracers(structure_name, loop_time, f_tracer, finest_ln, l_data_manager);
    std::vector<LagPoint> marker_position = tracers.getPositions();
    Pointer<IBExplicitHierarchyIntegrator> explicit_time_integrator = time_integrator;
    EigenAlignedVector<IBTK::Point> tracer_positions;
    for (auto it = marker_position.begin(); it != marker_position.end(); ++it)
    {
        LagPoint& X = *it;
        tracer_positions.emplace_back(X.x, X.y);
    }                
    explicit_time_integrator->setMarkers(tracer_positions); 
    explicit_time_integrator->writeMarkerPlotData(iteration_num, loop_time);

#pragma endregion
*/
            pout << "\n";
            pout << "At end       of timestep # " << iteration_num << "\n";
            pout << "Simulation time is " << loop_time << "\n";
            pout << "+++++++++++++++++++++++++++++++++++++++++++++++++++\n";
            pout << "\n";
            tbox::Pointer<hier::Variable<NDIM> > u_var = time_integrator->getVelocityVariable();
            tbox::Pointer<VariableContext> current_ctx = time_integrator->getCurrentContext();

            // At specified intervals, write visualization and restart files,
            // print out timer data, and store hierarchy data for post
            // processing.
            iteration_num += 1;
            const bool last_step = !time_integrator->stepsRemaining();
            if (dump_viz_data && (iteration_num % viz_dump_interval == 0 || last_step))
            {
                pout << "\nWriting visualization files...\n\n";
                if (uses_visit)
                {
                    time_integrator->setupPlotData();
                    visit_data_writer->writePlotData(patch_hierarchy, iteration_num, loop_time);
                    silo_data_writer->writePlotData(iteration_num, loop_time);
                    // Pointer<IBExplicitHierarchyIntegrator> explicit_time_integrator = time_integrator;
                    // explicit_time_integrator->setMarkers(tracer_positions);                    
                }
            }
            if (dump_restart_data && (iteration_num % restart_dump_interval == 0 || last_step))
            {
                pout << "\nWriting restart files...\n\n";
                RestartManager::getManager()->writeRestartFile(restart_dump_dirname, iteration_num);
            }
            if (dump_timer_data && (iteration_num % timer_dump_interval == 0 || last_step))
            {
                pout << "\nWriting timer data...\n\n";
                TimerManager::getManager()->print(plog);
            }

            {
                VariableDatabase<NDIM>* var_db = VariableDatabase<NDIM>::getDatabase();
                tbox::Pointer<hier::Variable<NDIM> > u_var = time_integrator->getVelocityVariable();
                const tbox::Pointer<VariableContext> u_ctx = time_integrator->getCurrentContext();
                const int u_idx = var_db->mapVariableAndContextToIndex(u_var, u_ctx);
                const int coarsest_ln = 0;
                const int finest_ln = patch_hierarchy->getFinestLevelNumber();
                HierarchyMathOps hier_math_ops("hier_math_ops", patch_hierarchy);
                hier_math_ops.setPatchHierarchy(patch_hierarchy);
                hier_math_ops.resetLevels(coarsest_ln, finest_ln);
                const int wgt_sc_idx = hier_math_ops.getSideWeightPatchDescriptorIndex();

                compute_flow_rate(patch_hierarchy, u_idx, loop_time, wgt_sc_idx);
            }
    // if (dump_viz_data && uses_visit && (iteration_num % viz_dump_interval == 0 || last_step))
    // {
    //     computeMarkerDisplacement(patch_hierarchy, ib_method_ops);
    // }
            pout << "\nWriting state data...\n\n";

        }

        
        if (dump_viz_data && uses_visit)
        {
            navier_stokes_integrator->setupPlotData();
            visit_data_writer->writePlotData(patch_hierarchy, iteration_num + 1, loop_time);
            // Pointer<IBExplicitHierarchyIntegrator> explicit_time_integrator = time_integrator;
            // explicit_time_integrator->setMarkers(tracer_positions);            
        }

        if (SAMRAI_MPI::getRank() == 0)
        {
            flow_rate_stream.close();
        }
        for (unsigned int d = 0; d < NDIM; ++d) delete u_bc_coefs[d];

    } // cleanup dynamically allocated objects prior to shutdown

    SAMRAIManager::shutdown();
    return 0;
} // main

void
compute_velocity_profile(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                         const int u_idx,
                         const double /*data_time*/,
                         const string& /*data_dump_dirname*/)
{
    const int coarsest_ln = 0;
    const int finest_ln = patch_hierarchy->getFinestLevelNumber();

    HierarchyMathOps hier_math_ops("hier_math_ops", patch_hierarchy);
    hier_math_ops.resetLevels(finest_ln, finest_ln);
    const int wgt_cc_idx = hier_math_ops.getCellWeightPatchDescriptorIndex();
    const double X_min[2] = { 0.1 * L, y_loc_min };
    const double X_max[2] = { 0.9 * L, y_loc_max };
    int qp_tot = 0;
    double u_Eulerian_L2_norm = 0.0;
    double u_Eulerian_max_norm = 0.0;
    vector<double> pos_values;
    for (int ln = finest_ln; ln >= coarsest_ln; --ln)
    {
        tbox::Pointer<PatchLevel<NDIM> > level = patch_hierarchy->getPatchLevel(ln);
        for (PatchLevel<NDIM>::Iterator p(level); p; p++)
        {
            tbox::Pointer<Patch<NDIM> > patch = level->getPatch(p());
            const Box<NDIM>& patch_box = patch->getBox();
            const CellIndex<NDIM>& patch_lower = patch_box.lower();
            const CellIndex<NDIM>& patch_upper = patch_box.upper();

            const tbox::Pointer<CartesianPatchGeometry<NDIM> > patch_geom = patch->getPatchGeometry();
            const double* const patch_x_lower = patch_geom->getXLower();
            const double* const patch_x_upper = patch_geom->getXUpper();

            const double* const patch_dx = patch_geom->getDx();

            // Entire box containing the required data.
            Box<NDIM> box(IndexUtilities::getCellIndex(
                              &X_min[0], patch_x_lower, patch_x_upper, patch_dx, patch_lower, patch_upper),
                          IndexUtilities::getCellIndex(
                              &X_max[0], patch_x_lower, patch_x_upper, patch_dx, patch_lower, patch_upper));
            // Part of the box on this patch
            Box<NDIM> trim_box = patch_box * box;
            BoxList<NDIM> iterate_box_list = trim_box;

            // Trim the box covered by the finer region
            BoxList<NDIM> covered_boxes;
            if (ln < finest_ln)
            {
                BoxArray<NDIM> refined_region_boxes;
                tbox::Pointer<PatchLevel<NDIM> > next_finer_level = patch_hierarchy->getPatchLevel(ln + 1);
                refined_region_boxes = next_finer_level->getBoxes();
                refined_region_boxes.coarsen(next_finer_level->getRatioToCoarserLevel());
                for (int i = 0; i < refined_region_boxes.getNumberOfBoxes(); ++i)
                {
                    const Box<NDIM> refined_box = refined_region_boxes[i];
                    const Box<NDIM> covered_box = trim_box * refined_box;
                    covered_boxes.unionBoxes(covered_box);
                }
            }
            iterate_box_list.removeIntersections(covered_boxes);

            // Loop over the boxes and store the location and interpolated value.
            tbox::Pointer<SideData<NDIM, double> > u_data = patch->getPatchData(u_idx);
            const tbox::Pointer<CellData<NDIM, double> > wgt_cc_data = patch->getPatchData(wgt_cc_idx);

            for (BoxList<NDIM>::Iterator lit(iterate_box_list); lit; lit++)
            {
                const Box<NDIM>& iterate_box = *lit;
                for (Box<NDIM>::Iterator bit(iterate_box); bit; bit++)
                {
                    const CellIndex<NDIM>& lower_idx = *bit;

                    const double yu = patch_x_lower[1] + patch_dx[1] * (lower_idx(1) - patch_lower(1) + 0.5);
                    const double zu = patch_x_lower[2] + patch_dx[2] * (lower_idx(2) - patch_lower(2) + 0.5);
                    const double xu = patch_x_lower[0] + patch_dx[0] * (lower_idx(0) - patch_lower(0));

                    double u_ex, v_ex, w_ex;

                    if (sqrt((yu - 0.5 * H) * (yu - 0.5 * H) + (zu - 0.5 * H) * (zu - 0.5 * H)) >= D / 2)
                    {
                        u_ex = 0.0;
                        v_ex = 0.0;
                        w_ex = 0.0;
                    }
                    else
                    {
                        u_ex =
                            U_MAX *
                            (1.0 - 4.0 * ((yu - 0.5 * H) * (yu - 0.5 * H) + (zu - 0.5 * H) * (zu - 0.5 * H)) / (D * D));
                        v_ex = 0.0;
                        w_ex = 0.0;
                    }

                    const double u0 = (*u_data)(SideIndex<NDIM>(lower_idx, 0, SideIndex<NDIM>::Lower));
                    const double v0 = (*u_data)(SideIndex<NDIM>(lower_idx, 1, SideIndex<NDIM>::Lower));
                    const double w0 = (*u_data)(SideIndex<NDIM>(lower_idx, 2, SideIndex<NDIM>::Lower));

                    if (xu > 0.4 * L && xu < 0.6 * L)
                    {
                        qp_tot += 1;
                        u_Eulerian_L2_norm += std::abs(u0 - u_ex) * std::abs(u0 - u_ex) * (*wgt_cc_data)(lower_idx);
                        u_Eulerian_L2_norm += std::abs(v0 - v_ex) * std::abs(v0 - v_ex) * (*wgt_cc_data)(lower_idx);
                        u_Eulerian_L2_norm += std::abs(w0 - w_ex) * std::abs(w0 - w_ex) * (*wgt_cc_data)(lower_idx);

                        u_Eulerian_max_norm = std::max(u_Eulerian_max_norm, std::abs(u0 - u_ex));
                        u_Eulerian_max_norm = std::max(u_Eulerian_max_norm, std::abs(v0 - v_ex));
                        u_Eulerian_max_norm = std::max(u_Eulerian_max_norm, std::abs(w0 - w_ex));
                    }
                }
            }
        }
    }

    SAMRAI_MPI::sumReduction(&qp_tot, 1);
    SAMRAI_MPI::sumReduction(&u_Eulerian_L2_norm, 1);
    SAMRAI_MPI::maxReduction(&u_Eulerian_max_norm, 1);
    u_Eulerian_L2_norm = sqrt(u_Eulerian_L2_norm);

    pout << " u_Eulerian_L2_norm = " << u_Eulerian_L2_norm << "\n\n";
    pout << " u_Eulerian_max_norm = " << u_Eulerian_max_norm << "\n\n";

    return;
} // compute_velocity_profile

void
output_data(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
            tbox::Pointer<INSHierarchyIntegrator> navier_stokes_integrator,
            const int iteration_num,
            const double loop_time,
            const string& data_dump_dirname)
{
    plog << "writing hierarchy data at iteration " << iteration_num << " to disk" << endl;
    plog << "simulation time is " << loop_time << endl;
    string file_name = data_dump_dirname + "/" + "hier_data.";
    char temp_buf[128];
    sprintf(temp_buf, "%05d.samrai.%05d", iteration_num, SAMRAI_MPI::getRank());
    file_name += temp_buf;
    tbox::Pointer<HDFDatabase> hier_db = new HDFDatabase("hier_db");
    hier_db->create(file_name);
    VariableDatabase<NDIM>* var_db = VariableDatabase<NDIM>::getDatabase();
    ComponentSelector hier_data;
    hier_data.setFlag(var_db->mapVariableAndContextToIndex(navier_stokes_integrator->getVelocityVariable(),
                                                           navier_stokes_integrator->getCurrentContext()));
    hier_data.setFlag(var_db->mapVariableAndContextToIndex(navier_stokes_integrator->getPressureVariable(),
                                                           navier_stokes_integrator->getCurrentContext()));
    patch_hierarchy->putToDatabase(hier_db->putDatabase("PatchHierarchy"), hier_data);
    hier_db->putDouble("loop_time", loop_time);
    hier_db->putInteger("iteration_num", iteration_num);
    hier_db->close();
    return;
} // output_data


void
compute_pressure_profile(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy,
                         const int p_idx,
                         const double /*data_time*/,
                         const string& /*data_dump_dirname*/)
{
    const int coarsest_ln = 0;
    const int finest_ln = patch_hierarchy->getFinestLevelNumber();

    HierarchyMathOps hier_math_ops("hier_math_ops", patch_hierarchy);
    hier_math_ops.resetLevels(finest_ln, finest_ln);
    const int wgt_cc_idx = hier_math_ops.getCellWeightPatchDescriptorIndex();

    const double X_min[2] = { x_loc_min, y_loc_min };
    const double X_max[2] = { x_loc_max, y_loc_max };
    // vector<double> pos_values;
    double p_Eulerian_L2_norm = 0.0;
    double p_Eulerian_max_norm = 0.0;

    int qp_tot = 0;
    for (int ln = finest_ln; ln >= coarsest_ln; --ln)
    {
        tbox::Pointer<PatchLevel<NDIM> > level = patch_hierarchy->getPatchLevel(ln);
        for (PatchLevel<NDIM>::Iterator p(level); p; p++)
        {
            tbox::Pointer<Patch<NDIM> > patch = level->getPatch(p());
            const Box<NDIM>& patch_box = patch->getBox();
            const CellIndex<NDIM>& patch_lower = patch_box.lower();
            const CellIndex<NDIM>& patch_upper = patch_box.upper();

            const tbox::Pointer<CartesianPatchGeometry<NDIM> > patch_geom = patch->getPatchGeometry();
            const double* const patch_x_lower = patch_geom->getXLower();
            const double* const patch_x_upper = patch_geom->getXUpper();
            const double* const patch_dx = patch_geom->getDx();

            // Entire box containing the required data.
            Box<NDIM> box(IndexUtilities::getCellIndex(
                              &X_min[0], patch_x_lower, patch_x_upper, patch_dx, patch_lower, patch_upper),
                          IndexUtilities::getCellIndex(
                              &X_max[0], patch_x_lower, patch_x_upper, patch_dx, patch_lower, patch_upper));
            // Part of the box on this patch
            Box<NDIM> trim_box = patch_box * box;
            BoxList<NDIM> iterate_box_list = trim_box;

            // Trim the box covered by the finer region
            BoxList<NDIM> covered_boxes;
            if (ln < finest_ln)
            {
                BoxArray<NDIM> refined_region_boxes;
                tbox::Pointer<PatchLevel<NDIM> > next_finer_level = patch_hierarchy->getPatchLevel(ln + 1);
                refined_region_boxes = next_finer_level->getBoxes();
                refined_region_boxes.coarsen(next_finer_level->getRatioToCoarserLevel());
                for (int i = 0; i < refined_region_boxes.getNumberOfBoxes(); ++i)
                {
                    const Box<NDIM> refined_box = refined_region_boxes[i];
                    const Box<NDIM> covered_box = trim_box * refined_box;
                    covered_boxes.unionBoxes(covered_box);
                }
            }
            iterate_box_list.removeIntersections(covered_boxes);

            // Loop over the boxes and store the location and interpolated value.
            const tbox::Pointer<CellData<NDIM, double> > p_data = patch->getPatchData(p_idx);
            const tbox::Pointer<CellData<NDIM, double> > wgt_cc_data = patch->getPatchData(wgt_cc_idx);

            for (BoxList<NDIM>::Iterator lit(iterate_box_list); lit; lit++)
            {
                const Box<NDIM>& iterate_box = *lit;
                for (Box<NDIM>::Iterator bit(iterate_box); bit; bit++)
                {
                    const CellIndex<NDIM>& cell_idx = *bit;
                    const double p1 = (*p_data)(cell_idx);
                    const double y = patch_x_lower[1] + patch_dx[1] * (cell_idx(1) - patch_lower(1) + 0.5);
                    const double z = patch_x_lower[2] + patch_dx[2] * (cell_idx(2) - patch_lower(2) + 0.5);
                    const double x = patch_x_lower[0] + patch_dx[0] * (cell_idx(0) - patch_lower(0) + 0.5);

                    double p_ex_qp;
                    if (sqrt((y - 0.5 * H) * (y - 0.5 * H) + (z - 0.5 * H) * (z - 0.5 * H)) > D / 2 + patch_dx[1])
                    {
                        p_ex_qp = 0.0;
                    }
                    else if (sqrt((y - 0.5 * H) * (y - 0.5 * H) + (z - 0.5 * H) * (z - 0.5 * H)) < D / 2 - patch_dx[1])
                    {
                        p_ex_qp = -2.0 * dp * x / L + dp;
                    }
                    else
                    {
                        p_ex_qp = p1;
                    }

                    if (x > 0.2 * L && x < 0.8 * L)
                    {
                        qp_tot += 1;
                        p_Eulerian_L2_norm +=
                            std::abs(p1 - p_ex_qp) * std::abs(p1 - p_ex_qp) * (*wgt_cc_data)(cell_idx);
                        p_Eulerian_max_norm = std::max(p_Eulerian_max_norm, std::abs(p1 - p_ex_qp));
                    }
                }
            }
        }
    }
    SAMRAI_MPI::sumReduction(&qp_tot, 1);
    SAMRAI_MPI::sumReduction(&p_Eulerian_L2_norm, 1);
    SAMRAI_MPI::maxReduction(&p_Eulerian_max_norm, 1);

    p_Eulerian_L2_norm = sqrt(p_Eulerian_L2_norm);

    pout << " p_Eulerian_L2_norm = " << p_Eulerian_L2_norm << "\n\n";
    pout << " p_Eulerian_max_norm = " << p_Eulerian_max_norm << "\n\n";

    return;
} // compute_pressure_profile

void
compute_flow_rate(const tbox::Pointer<PatchHierarchy<NDIM> > hierarchy,
                  const int U_idx,
                  const double loop_time,
                  const int wgt_sc_idx)
{
    vector<double> qsrc;
    qsrc.resize(2);
    std::fill(qsrc.begin(), qsrc.end(), 0.0);
    const double posni[2] = { 0.0, yi };
    const double posno[2] = { L, yo };
    const double rsrc[2] = { 0.5 * D, 0.5 * D };
    for (int ln = 0; ln <= hierarchy->getFinestLevelNumber(); ++ln)
    {
        tbox::Pointer<PatchLevel<NDIM> > level = hierarchy->getPatchLevel(ln);
        for (PatchLevel<NDIM>::Iterator p(level); p; p++)
        {
            tbox::Pointer<Patch<NDIM> > patch = level->getPatch(p());
            tbox::Pointer<CartesianPatchGeometry<NDIM> > pgeom = patch->getPatchGeometry();
            if (pgeom->getTouchesRegularBoundary())
            {
                tbox::Pointer<SideData<NDIM, double> > U_data = patch->getPatchData(U_idx);
                tbox::Pointer<SideData<NDIM, double> > wgt_sc_data = patch->getPatchData(wgt_sc_idx);
                const Box<NDIM>& patch_box = patch->getBox();
                const double* const x_lower = pgeom->getXLower();
                const double* const dx = pgeom->getDx();
                double dV = 1.0;
                for (int d = 0; d < NDIM; ++d)
                {
                    dV *= dx[d];
                }
                double X[NDIM];
                static const int axis = 0;
                for (int side = 0; side <= 1; ++side)
                {
                    const bool is_lower = side == 0;
                    if (pgeom->getTouchesRegularBoundary(axis, side))
                    {
                        // const double rsrc = d_rsrc[side];
                        //
                        VectorNd n;
                        for (int d = 0; d < NDIM; ++d)
                        {
                            n[d] = axis == d ? (is_lower ? -1.0 : +1.0) : 0.0;
                        }
                        Box<NDIM> side_box = patch_box;
                        if (is_lower)
                        {
                            side_box.lower(axis) = patch_box.lower(axis);
                            side_box.upper(axis) = patch_box.lower(axis);
                        }
                        else
                        {
                            side_box.lower(axis) = patch_box.upper(axis) + 1;
                            side_box.upper(axis) = patch_box.upper(axis) + 1;
                        }
                        for (Box<NDIM>::Iterator b(side_box); b; b++)
                        {
                            const hier::Index<NDIM>& i = b();
                            double r_sq = 0.0;
                            for (int d = 0; d < NDIM; ++d)
                            {
                                X[d] =
                                    x_lower[d] + dx[d] * (double(i(d) - patch_box.lower(d)) + (d == axis ? 0.0 : 0.5));
                                if (d != axis && side == 0) r_sq += pow(X[d] - posni[d], 2.0);
                                if (d != axis && side == 1) r_sq += pow(X[d] - posno[d], 2.0);
                            }
                            const double r = sqrt(r_sq);
                            if (r <= rsrc[side])
                            {
                                const SideIndex<NDIM> i_s(i, axis, SideIndex<NDIM>::Lower);
                                if ((*wgt_sc_data)(i_s) > std::numeric_limits<double>::epsilon())
                                {
                                    double dA = n[axis] * dV / dx[axis];
                                    qsrc[side] += (*U_data)(i_s)*dA;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    SAMRAI_MPI::sumReduction(&qsrc[0], 2);

    if (SAMRAI_MPI::getRank() == 0)
    {
        flow_rate_stream << loop_time << " " << qsrc[0] << " " << qsrc[1] << endl;
    }

    pout << " Qi = " << qsrc[0] << " Qo = " << qsrc[1] << "\n\n";
}


void computeMarkerDisplacement(tbox::Pointer<PatchHierarchy<NDIM> > patch_hierarchy, Pointer<IBMethod> ib_method_ops)
{
/*
    const int finest_ln = patch_hierarchy->getFinestLevelNumber();
    Pointer<LData> X_data =
        ib_method_ops->getLDataManager()->getLData("X", finest_ln); // Lagrangian data at current location
    Pointer<LData> X0_data = ib_method_ops->getLDataManager()->getLData(
        LDataManager::INIT_POSN_DATA_NAME, finest_ln); // Lagrangian data at initial location
    Vec X_vec = X_data->getVec();
    Vec X0_vec = X0_data->getVec();
    double* x_vals;
    double* x0_vals;
    int ierr = VecGetArray(X_vec, &x_vals);
    int ierr2 = VecGetArray(X0_vec, &x0_vals);
    IBTK_CHKERRQ(ierr);
    IBTK_CHKERRQ(ierr2);
    Pointer<LMesh> l_mesh = ib_method_ops->getLDataManager()->getLMesh(finest_ln);
    const std::vector<LNode*>& local_nodes = l_mesh->getLocalNodes();
    int num_nodes = local_nodes.size();

    // Send the number of ib nodes on each processor to the root processor 0
    if (rank != 0)
    {
        MPI_Send(&num_nodes, 1, MPI_INT, 0, 0, IBTK_MPI::getCommunicator());
    }
    else
    {
        nodes_per_proc[0] = num_nodes;

        for (int r = 1; r < n_proc; ++r)
        {
            MPI_Recv(&nodes_per_proc[r], 1, MPI_INT, r, 0, IBTK_MPI::getCommunicator(), MPI_STATUS_IGNORE);
        }
    }
    IBTK_MPI::barrier();

    // initialize vector to store the nodal coordinates on each processor
    std::vector<Lag_Coords> local_coordinates;
    std::vector<Lag_Forces> local_forces;
    for (const auto& node : local_nodes)
    {
        const int petsc_idx = node->getLocalPETScIndex();
        const int lag_idx = node->getLagrangianIndex();
        // put the lag idx as the first entry of the local_coordinates vector since the petsc indices may
        // change throughout a computation
        Eigen::Map<Vector3d> X(&x_vals[petsc_idx * NDIM]); // X is a vector containing the local coordinates
        Eigen::Map<Vector3d> X0(&x0_vals[petsc_idx * NDIM]); // X0 is a vector containing the local
                                                            // coordinates of the initial data

        local_coordinates.push_back({ lag_idx, X[0], X[1]});
        local_forces.push_back({ lag_idx,kappa*(X0[0]-X[0]), kappa*(X0[1]-X[1])});
        double dist = sqrt((X0[0]-X[0])*(X0[0]-X[0]) + (X0[1]-X[1])*(X0[1]-X[1]));
        if (dist > constraint)
        {
            TBOX_ERROR("The IB points move more than Cole hoped");
        }
    }
*/    
}
