// Copyright (c) 2002-2014, Boyce Griffith
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of The University of North Carolina nor the names of
//      its contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

// Config files
// #include <IBAMR_config.h>
// #include <IBTK_config.h>
#include <SAMRAI_config.h>

// Headers for basic PETSc functions
#include <petscsys.h>

// Headers for basic SAMRAI objects
#include <BergerRigoutsos.h>
#include <CartesianGridGeometry.h>
#include <LoadBalancer.h>
#include <StandardTagAndInitialize.h>

// Headers for basic libMesh objects
#include <libmesh/boundary_info.h>
#include <libmesh/equation_systems.h>
#include <libmesh/exodusII_io.h>
#include <libmesh/mesh.h>
#include <libmesh/mesh_generation.h>


//////////////////////////////////////////////////

#include <algorithm>
#include <cmath>
#include <iostream>

#include "libmesh/analytic_function.h"
#include "libmesh/dense_matrix.h"
#include "libmesh/dense_vector.h"
#include "libmesh/dirichlet_boundaries.h"
#include "libmesh/dof_map.h"
#include "libmesh/elem.h"
#include "libmesh/equation_systems.h"
#include "libmesh/exodusII_io.h"
//#include "libmesh/explicit_system.h"
#include "libmesh/fe.h"
#include "libmesh/fe_map.h"
#include "libmesh/getpot.h"
#include "libmesh/gmv_io.h"
#include "libmesh/libmesh.h"
#include "libmesh/linear_implicit_system.h"
#include "libmesh/mesh.h"
#include "libmesh/mesh_generation.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/quadrature_gauss.h"
#include "libmesh/quadrature_trap.h"
#include "libmesh/sparse_matrix.h"
#include "libmesh/string_to_enum.h"
#include "libmesh/tensor_value.h"
#include "libmesh/vector_value.h"

//////////////////////////////////////////////////


// Headers for application-specific algorithm/data structure objects
#include <boost/multi_array.hpp>
#include <ibamr/IBExplicitHierarchyIntegrator.h>
#include <ibamr/IBFEMethod.h>
#include <ibamr/INSCollocatedHierarchyIntegrator.h>
#include <ibamr/INSStaggeredHierarchyIntegrator.h>
#include <ibtk/AppInitializer.h>
#include <ibtk/libmesh_utilities.h>
#include <ibtk/muParserCartGridFunction.h>
#include <ibtk/muParserRobinBcCoefs.h>

// Set up application namespace declarations
#include <ibamr/app_namespaces.h>


// Elasticity model data.
namespace ModelData
{
  
static BoundaryInfo* lag_bdry_info;
  
// Tether (penalty) force function for the solid blocks.
static double kappa = 1.0e6;
static double damping = 0.0;
static double traction_force = 6.25e3;
static double load_time;
static double PI = 3.141592653589793;

static bool use_volumetric_term;
static std::string stress_funtion;

double time_ramp(double time);

void
solid_surface_force_function(VectorValue<double>& F,
			    const VectorValue<double>& /*n*/,
			    const VectorValue<double>& /*N*/,
                            const TensorValue<double>& /*FF*/,
                            const libMesh::Point& X,
                            const libMesh::Point& s,
                            Elem* const elem,
			    const unsigned short int side,
                            const vector<const vector<double>*>& /*var_data*/,
                            const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
                            double time,
                            void* /*ctx*/)
{   
    if (lag_bdry_info->has_boundary_id(elem, side, 3))
    {
      F = kappa * (s - X);
    }
    else if (lag_bdry_info->has_boundary_id(elem, side, 1))
    {
      F = libMesh::Point(0.0, time_ramp(time) * traction_force, 0.0);
    }
    else
    {
      F.zero();
    }
    return;
} // solid_surface_force_function



static double shear_mod, bulk_mod;
void
PK1_dev_stress_function_mod(TensorValue<double>& PP,
                        const TensorValue<double>& FF,
                        const libMesh::Point& /*X*/,
                        const libMesh::Point& /*s*/,
                        Elem* const /*elem*/,
                        const vector<const vector<double>*>& /*var_data*/,
                        const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
                        double /*time*/,
                        void* /*ctx*/)
{
    RealTensor C = FF.transpose()*FF;
    double I1    = C.tr();

    //modified
    RealTensor dI1_bar_dFF = pow(FF.det(), -2.0 / 3.0) * (FF - I1 / 3.0 * tensor_inverse_transpose(FF, NDIM));
    PP = shear_mod * dI1_bar_dFF;

    return;
} // PK1_dev_stress_function


void
PK1_dev_stress_function_unmod(TensorValue<double>& PP,
			      const TensorValue<double>& FF,
			      const libMesh::Point& /*X*/,
			      const libMesh::Point& /*s*/,
			      Elem* const /*elem*/,
			      const vector<const vector<double>*>& /*var_data*/,
			      const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
			      double /*time*/,
			      void* /*ctx*/)
{
    
    //unmodified     
    PP = shear_mod * FF;

    return;
} // PK1_dev_stress_function

void
PK1_dev_stress_function_dev(TensorValue<double>& PP,
			    const TensorValue<double>& FF,
			    const libMesh::Point& /*X*/,
			    const libMesh::Point& /*s*/,
			    Elem* const /*elem*/,
			    const vector<const vector<double>*>& /*var_data*/,
			    const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
			    double /*time*/,
			    void* /*ctx*/)
{
    RealTensor C = FF.transpose()*FF;
    double I1    = C.tr();

    //deviatoric projection
    PP = shear_mod * (FF - I1 / 3.0 * tensor_inverse_transpose(FF, NDIM));
    
    return;
} // PK1_dev_stress_function


void
PK1_dil_stress_function(TensorValue<double>& PP,
                        const TensorValue<double>& FF,
                        const libMesh::Point& /*X*/,
                        const libMesh::Point& /*s*/,
                        Elem* const /*elem*/,
                        const vector<const vector<double>*>& /*var_data*/,
                        const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
                        double /*time*/,
                        void* /*ctx*/)
{
    PP = bulk_mod * log(FF.det()) * tensor_inverse_transpose(FF, NDIM);
    return;
} // PK1_dil_stress_function

void
solid_body_force_function(VectorValue<double>& F,
			  const TensorValue<double>& /*FF*/,
			  const libMesh::Point& /*X*/,
			  const libMesh::Point& /*s*/,
			  Elem* const elem,
			  const vector<const vector<double>*>& var_data,
			  const vector<const vector<VectorValue<double> >*>& /*grad_var_data*/,
			  double time,
			  void* /*ctx*/)
{   
  
    const vector<double>& U = *var_data[0];
    for (unsigned int d = 0; d < NDIM; d++)
    {
      F(d) = -damping * U[d];
    }
    return;
  

} // solid_body_force_function


Real
initial_jacobian(const libMesh::Point& /*p*/,
                 const Parameters& /*parameters*/,
                 const string& /*system*/,
                 const string& var_name)
{
    // could include something more complicated
    if (var_name == "Avg J")
      return 1.0;
    else
      return 0.0;
}

void apply_initial_jacobian(EquationSystems& es, const string& system_name)
{
    libmesh_assert_equal_to(system_name, "JacobianDeterminant");
    ExplicitSystem& system = es.get_system<ExplicitSystem>("JacobianDeterminant");
    es.parameters.set<Real>("time") = system.time = 0;
    system.project_solution(initial_jacobian, NULL, es.parameters);
    return;
}


double time_ramp(double time)
{
  
  double displacement_factor;
  if (time < load_time)
//     displacement_factor = sin(PI * time / load_time / 2.0);
    displacement_factor = -2.0 * (time/load_time)*(time/load_time)*(time/load_time)  + 3.0 * (time/load_time)*(time/load_time);
  else
    displacement_factor = 1.0;
  
  return displacement_factor;
  
}

}
using namespace ModelData;

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
    //Initialize libMesh, PETSc, MPI, and SAMRAI.
    LibMeshInit init(argc, argv);
    SAMRAI_MPI::setCommunicator(PETSC_COMM_WORLD);
    //  SAMRAI_MPI::setCallAbortInSerialInsteadOfExit();
    SAMRAIManager::startup();

    { // cleanup dynamically allocated objects prior to shutdown

        // Parse command line options, set some standard options from the input
        // file, initialize the restart database (if this is a restarted run),
        // and enable file logging.
        Pointer<AppInitializer> app_initializer = new AppInitializer(argc, argv, "IB.log");
        Pointer<Database> input_db = app_initializer->getInputDatabase();

        // Get various standard options set in the input file.
        const bool dump_viz_data = app_initializer->dumpVizData();
        const int viz_dump_interval = app_initializer->getVizDumpInterval();
        const bool uses_visit = dump_viz_data && app_initializer->getVisItDataWriter();
        const bool uses_exodus = dump_viz_data && !app_initializer->getExodusIIFilename().empty();
        const string exodus_filename = app_initializer->getExodusIIFilename();

        const bool dump_restart_data = app_initializer->dumpRestartData();
        const int restart_dump_interval = app_initializer->getRestartDumpInterval();
        const string restart_dump_dirname = app_initializer->getRestartDumpDirectory();

        const bool dump_timer_data = app_initializer->dumpTimerData();
        const int timer_dump_interval = app_initializer->getTimerDumpInterval();

        // Create a simple FE mesh.
        const double dx = input_db->getDouble("DX");
	const double ds = input_db->getDouble("MFAC") * dx;
	const string elem_type = input_db->getStringWithDefault("ELEM_TYPE", "TRI3");
	
	
	Mesh mesh(init.comm(), NDIM);
	
	if (input_db->keyExists("MESH_NAME"))
	{
	  mesh.read(input_db->getString("MESH_NAME"));
	}
	else
	{
	
	  int ps_x        = input_db->getDouble("M");    //ceil(2*1.0 / ds);
	  int ps_y        = ps_x; //ceil(2*halfwidth / ds);
	  int ps_z        = ps_x; //ceil(2*halfheight / ds);
	
	  // Build grid mesh
	  MeshTools::Generation::build_cube(mesh,
					    ps_x,
					    (NDIM > 1) ? ps_y : 0,
					    (NDIM > 2) ? ps_z : 0,
					    0.0,1.0,
					    0.0,1.0,
					    0.0,1.0,
					    Utility::string_to_enum<ElemType>(elem_type));
	  
	  //Map unit square onto cook's membrane
	  MeshBase::const_node_iterator nd = mesh.nodes_begin();
	  const MeshBase::const_node_iterator end_nd = mesh.nodes_end();
	  for (; nd != end_nd; ++nd)
	  {
	    libMesh::Point s = **nd;
	    (**nd)(0) =  4.8 * s(0) + 2.6;
	    (**nd)(1) = -2.8 * s(0) * s(1) + 4.4 * s(0) + 4.4 * s(1) + 2.0;
	  }
	  
	}



	mesh.prepare_for_use();
	lag_bdry_info = &mesh.get_boundary_info();

        shear_mod      = input_db->getDouble("SHEAR_MOD");
	bulk_mod       = input_db->getDouble("BULK_MOD");
	damping        = input_db->getDouble("ETA");
	kappa          = input_db->getDouble("KAPPA");
	traction_force = input_db->getDouble("TRACTION_FORCE");
	load_time         = input_db->getDouble("LOAD_TIME");
	
	use_volumetric_term = input_db->getBool("USE_VOLUMETRIC_TERM");
	stress_funtion     = input_db->getString("STRESS_FUNCTION");

        // Create major algorithm and data objects that comprise the
        // application.  These objects are configured from the input database
        // and, if this is a restarted run, from the restart database.
        Pointer<INSHierarchyIntegrator> navier_stokes_integrator = new INSStaggeredHierarchyIntegrator(
            "INSStaggeredHierarchyIntegrator",
            app_initializer->getComponentDatabase("INSStaggeredHierarchyIntegrator"));
        Pointer<IBFEMethod> ib_method_ops =
            new IBFEMethod("IBFEMethod",
                           app_initializer->getComponentDatabase("IBFEMethod"),
                           &mesh,
                           app_initializer->getComponentDatabase("GriddingAlgorithm")->getInteger("max_levels"));
        Pointer<IBHierarchyIntegrator> time_integrator =
            new IBExplicitHierarchyIntegrator("IBHierarchyIntegrator",
                                              app_initializer->getComponentDatabase("IBHierarchyIntegrator"),
                                              ib_method_ops,
                                              navier_stokes_integrator);
        Pointer<CartesianGridGeometry<NDIM> > grid_geometry = new CartesianGridGeometry<NDIM>(
            "CartesianGeometry", app_initializer->getComponentDatabase("CartesianGeometry"));
        Pointer<PatchHierarchy<NDIM> > patch_hierarchy = new PatchHierarchy<NDIM>("PatchHierarchy", grid_geometry);
        Pointer<StandardTagAndInitialize<NDIM> > error_detector =
            new StandardTagAndInitialize<NDIM>("StandardTagAndInitialize",
                                               time_integrator,
                                               app_initializer->getComponentDatabase("StandardTagAndInitialize"));
        Pointer<BergerRigoutsos<NDIM> > box_generator = new BergerRigoutsos<NDIM>();
        Pointer<LoadBalancer<NDIM> > load_balancer =
            new LoadBalancer<NDIM>("LoadBalancer", app_initializer->getComponentDatabase("LoadBalancer"));
        Pointer<GriddingAlgorithm<NDIM> > gridding_algorithm =
            new GriddingAlgorithm<NDIM>("GriddingAlgorithm",
                                        app_initializer->getComponentDatabase("GriddingAlgorithm"),
                                        error_detector,
                                        box_generator,
                                        load_balancer);

	//attach velocity
	std::vector<int> vars(NDIM);
	for (unsigned int d = 0; d < NDIM; ++d) vars[d] = d;
	vector<SystemData> velocity_data(1);
	velocity_data[0] = SystemData(IBFEMethod::VELOCITY_SYSTEM_NAME,vars);
	
        // Configure the IBFE solver.
        IBFEMethod::LagSurfaceForceFcnData solid_surface_force_data(solid_surface_force_function);
	IBFEMethod::LagBodyForceFcnData solid_body_force_data(solid_body_force_function,velocity_data);
	ib_method_ops->registerLagSurfaceForceFunction(solid_surface_force_data);
	ib_method_ops->registerLagBodyForceFunction(solid_body_force_data);        

	
	if (stress_funtion == "MODIFIED")
	{
	  IBFEMethod::PK1StressFcnData PK1_dev_stress_data(PK1_dev_stress_function_mod);
	  PK1_dev_stress_data.quad_order =
	      Utility::string_to_enum<libMesh::Order>(input_db->getStringWithDefault("PK1_DEV_QUAD_ORDER", "THIRD"));
	  ib_method_ops->registerPK1StressFunction(PK1_dev_stress_data);
	}
	else if (stress_funtion == "UNMODIFIED")
	{
	  IBFEMethod::PK1StressFcnData PK1_dev_stress_data(PK1_dev_stress_function_unmod);
	  PK1_dev_stress_data.quad_order =
	      Utility::string_to_enum<libMesh::Order>(input_db->getStringWithDefault("PK1_DEV_QUAD_ORDER", "THIRD"));
	  ib_method_ops->registerPK1StressFunction(PK1_dev_stress_data);
	}
	else if (stress_funtion == "DEVIATORIC")
	{
	  IBFEMethod::PK1StressFcnData PK1_dev_stress_data(PK1_dev_stress_function_dev);
	  PK1_dev_stress_data.quad_order =
	      Utility::string_to_enum<libMesh::Order>(input_db->getStringWithDefault("PK1_DEV_QUAD_ORDER", "THIRD"));
	  ib_method_ops->registerPK1StressFunction(PK1_dev_stress_data);
	}
	else
	{
	  IBFEMethod::PK1StressFcnData PK1_dev_stress_data(PK1_dev_stress_function_mod);
	  PK1_dev_stress_data.quad_order =
	      Utility::string_to_enum<libMesh::Order>(input_db->getStringWithDefault("PK1_DEV_QUAD_ORDER", "THIRD"));
	  ib_method_ops->registerPK1StressFunction(PK1_dev_stress_data);
	}
        
	if (use_volumetric_term == true)
	{
	  IBFEMethod::PK1StressFcnData PK1_dil_stress_data(PK1_dil_stress_function);
	  PK1_dil_stress_data.quad_order =
	      Utility::string_to_enum<libMesh::Order>(input_db->getStringWithDefault("PK1_DIL_QUAD_ORDER", "FIRST"));
	  ib_method_ops->registerPK1StressFunction(PK1_dil_stress_data);
	}

        ib_method_ops->initializeFEEquationSystems();
        EquationSystems* equation_systems = ib_method_ops->getFEDataManager()->getEquationSystems();
	ExplicitSystem& jac_system = equation_systems->add_system<ExplicitSystem>("JacobianDeterminant");
	unsigned int J_var = jac_system.add_variable("Avg J", CONSTANT, MONOMIAL);
	jac_system.attach_init_function(apply_initial_jacobian);

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

        // Create Eulerian boundary condition specification objects (when necessary).
        const IntVector<NDIM>& periodic_shift = grid_geometry->getPeriodicShift();
        vector<RobinBcCoefStrategy<NDIM>*> u_bc_coefs(NDIM);
        if (periodic_shift.min() > 0)
        {
            for (unsigned int d = 0; d < NDIM; ++d)
            {
                u_bc_coefs[d] = NULL;
            }
        }
        else
        {
            for (unsigned int d = 0; d < NDIM; ++d)
            {
                ostringstream bc_coefs_name_stream;
                bc_coefs_name_stream << "u_bc_coefs_" << d;
                const string bc_coefs_name = bc_coefs_name_stream.str();

                ostringstream bc_coefs_db_name_stream;
                bc_coefs_db_name_stream << "VelocityBcCoefs_" << d;
                const string bc_coefs_db_name = bc_coefs_db_name_stream.str();

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
        Pointer<VisItDataWriter<NDIM> > visit_data_writer = app_initializer->getVisItDataWriter();
        if (uses_visit)
        {
            time_integrator->registerVisItDataWriter(visit_data_writer);
        }
        unique_ptr<ExodusII_IO> exodus_io(uses_exodus ? new ExodusII_IO(mesh) : NULL);

        // Initialize hierarchy configuration and data on all patches.
        ib_method_ops->initializeFEData();
        time_integrator->initializePatchHierarchy(patch_hierarchy, gridding_algorithm);

        // Deallocate initialization objects.
        app_initializer.setNull();

        // Print the input database contents to the log file.
        plog << "Input database:\n";
        input_db->printClassData(plog);

        // Write out initial visualization data.
        int iteration_num = time_integrator->getIntegratorStep();
        double loop_time = time_integrator->getIntegratorTime();
        if (dump_viz_data)
        {
            pout << "\n\nWriting visualization files...\n\n";
            if (uses_visit)
            {
                time_integrator->setupPlotData();
                visit_data_writer->writePlotData(patch_hierarchy, iteration_num, loop_time);
            }
            if (uses_exodus)
            {
                exodus_io->write_timestep(
                    exodus_filename, *equation_systems, iteration_num / viz_dump_interval + 1, loop_time);
            }
        }

	// Open streams to save volume of structure.
        ofstream volume_stream;
        if (SAMRAI_MPI::getRank() == 0)
        {
            volume_stream.open("volume.curve", ios_base::out | ios_base::trunc);
        }


        // Main time step loop.
        double loop_time_end = time_integrator->getEndTime();
        double dt = 0.0;
        while (!MathUtilities<double>::equalEps(loop_time, loop_time_end) && time_integrator->stepsRemaining())
        {
            iteration_num = time_integrator->getIntegratorStep();
            loop_time = time_integrator->getIntegratorTime();

            pout << "\n";
            pout << "+++++++++++++++++++++++++++++++++++++++++++++++++++\n";
            pout << "At beginning of timestep # " << iteration_num << "\n";
            pout << "Simulation time is " << loop_time << "\n";
	     
	    
	    //to compute average J for each element
	    double J_integral = 0.0;
	    double L_1_error_J = 0.0;
	    double L_2_error_J = 0.0;
	    double L_oo_temp = 0.0;
	    double L_oo_error_J = 0.0;
	    double elem_area  = 0.0;
            System& X_system = equation_systems->get_system<System>(IBFEMethod::COORDS_SYSTEM_NAME);
            NumericVector<double>* X_vec = X_system.solution.get();
            NumericVector<double>* X_ghost_vec = X_system.current_local_solution.get();
            X_vec->localize(*X_ghost_vec);
            DofMap& X_dof_map = X_system.get_dof_map();
            vector<vector<unsigned int> > X_dof_indices(NDIM);
            unique_ptr<FEBase> fe(FEBase::build(NDIM, X_dof_map.variable_type(0)));
            unique_ptr<QBase> qrule = QBase::build(QGAUSS, NDIM, FIFTH);
            fe->attach_quadrature_rule(qrule.get());
            const vector<double>& JxW = fe->get_JxW();
            const vector<vector<VectorValue<double> > >& dphi = fe->get_dphi();
            TensorValue<double> FF;
            boost::multi_array<double, 2> X_node;
            const MeshBase::const_element_iterator el_begin = mesh.active_local_elements_begin();
            const MeshBase::const_element_iterator el_end = mesh.active_local_elements_end();
            for (MeshBase::const_element_iterator el_it = el_begin; el_it != el_end; ++el_it)
            {
	      
		elem_area  = 0.0;
		L_oo_temp = 0.0;
		J_integral = 0.0;
		
                Elem* const elem = *el_it;
                fe->reinit(elem);
                for (unsigned int d = 0; d < NDIM; ++d)
                {
                    X_dof_map.dof_indices(elem, X_dof_indices[d], d);
                }
                //std::cout << "\n Current element is " << elem;
                const int n_qp = qrule->n_points();
                get_values_for_interpolation(X_node, *X_ghost_vec, X_dof_indices);
                for (int qp = 0; qp < n_qp; ++qp)
                {
                    jacobian(FF, qp, X_node, dphi);
                    J_integral += abs(FF.det()) * JxW[qp];
		    L_1_error_J += abs(1.0 - FF.det()) * JxW[qp];
		    L_2_error_J += (1.0 - FF.det()) * (1.0 - FF.det()) * JxW[qp];
		    L_oo_temp += abs(1.0 - FF.det()) * JxW[qp];
		    //if (abs(1.0 - FF.det()) > L_oo_error_J ) L_oo_error_J = abs(1.0 - FF.det());
		    elem_area  += JxW[qp];
		    
		    //std::cout << "\n At qp, abs error in J is " << abs(1.0 - FF.det());
		    
                }
                //std::cout << "\n";
		L_oo_temp = L_oo_temp / elem_area;
		if (L_oo_temp > L_oo_error_J) L_oo_error_J = L_oo_temp;
		J_integral = J_integral / elem_area;
		ExplicitSystem& jac_system        = equation_systems->get_system<ExplicitSystem>("JacobianDeterminant");
		const unsigned int jac_system_num = jac_system.number();  // identifier number for the system
		const dof_id_type dof_id_J        = elem->dof_number(jac_system_num,0,/*comp*/0);
		jac_system.solution->set(dof_id_J, J_integral);
		//jac_system.solution->localize(*jac_system.current_local_solution);
                
            }
            jac_system.solution->close();
            L_1_error_J = SAMRAI_MPI::sumReduction(L_1_error_J);
	    L_2_error_J = SAMRAI_MPI::sumReduction(L_2_error_J);
	    L_oo_error_J = SAMRAI_MPI::maxReduction(L_oo_error_J);
	    if (SAMRAI_MPI::getRank() == 0)
	    L_2_error_J = sqrt(L_2_error_J);
	    
	    
	    
	    pout << "\nThe L_1 error of J = " << L_1_error_J << "\n";
	    pout << "\nThe L_2 error of J = " << L_2_error_J << "\n";
	    pout << "\nThe L_oo error of J = " << L_oo_error_J << "\n";
	    pout << "\n";
	    

            dt = time_integrator->getMaximumTimeStepSize();
            time_integrator->advanceHierarchy(dt);
            loop_time += dt;

            pout << "\n";
            pout << "At end       of timestep # " << iteration_num << "\n";
            pout << "Simulation time is " << loop_time << "\n";
            pout << "+++++++++++++++++++++++++++++++++++++++++++++++++++\n";
            pout << "\n";

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
                }
                if (uses_exodus)
                {
                    exodus_io->write_timestep(exodus_filename,
					      *equation_systems,
                                              iteration_num / viz_dump_interval + 1,
                                              loop_time);
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
            

     
        } //end of time loop

        // Cleanup Eulerian boundary condition specification objects (when
        // necessary).
        for (unsigned int d = 0; d < NDIM; ++d) delete u_bc_coefs[d];

    } // cleanup dynamically allocated objects prior to shutdown

    SAMRAIManager::shutdown();
    return 0;
} // main
