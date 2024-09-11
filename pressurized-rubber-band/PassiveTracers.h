#include <ibamr/AdvDiffHierarchyIntegrator.h>
#include <ibtk/LData.h>
#include <ibtk/LDataManager.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

// Struct to represent a point
struct LagPoint 
{
    double x, y;
    double angle; // Angle relative to the circle center
};

// Function to calculate the distance between two points
double distance(const LagPoint& p1, const LagPoint& p2) 
{
    return std::sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}

class PassiveTracers
{
public:
    PassiveTracers(const std::string& structure_name,
                   const double current_time,
                   std::ofstream& output_file,
                   const int finest_ln,
                   IBTK::LDataManager* l_data_manager)
        : d_structure_name(structure_name),
          d_current_time(current_time),
          d_output_file(output_file),       
          d_data_manager(l_data_manager)
    {
        const int structure_id = d_data_manager->getLagrangianStructureID(structure_name, finest_ln);
        std::pair<int, int> lag_idx_range = d_data_manager->getLagrangianStructureIndexRange(structure_id, finest_ln);

        // Get the Lagrangian mesh and local nodes for the finest level
        SAMRAI::tbox::Pointer<IBTK::LMesh> mesh = d_data_manager->getLMesh(finest_ln);
        const std::vector<IBTK::LNode*>& local_nodes = mesh->getLocalNodes();

        // Retrieve the Lagrangian data for the positions (X)
        SAMRAI::tbox::Pointer<IBTK::LData> ptr_x_lag_data = d_data_manager->getLData("X", finest_ln);
        const boost::multi_array_ref<double, 2>& X0_data = *ptr_x_lag_data->getLocalFormVecArray();

        // Loop through the local nodes
        for (const auto& node : local_nodes)
        {
            const int lag_idx = node->getLagrangianIndex();

            // Ensure the node belongs to the desired structure
            if (lag_idx_range.first <= lag_idx && lag_idx < lag_idx_range.second)
            {
                const int local_idx = node->getLocalPETScIndex();
                const double* const X = &X0_data[local_idx][0];

                LagPoint pt;
                pt.x = X[0];
                pt.y = X[1];
                pt.angle = 0.0;
                d_points.push_back(pt);
            }
        }                
    }

    // Calculate the center of the structure
    LagPoint getStructureCenter() const
    {
        LagPoint center = {0.0, 0.0};
        for (const auto& p : d_points) 
        {
            center.x += p.x;
            center.y += p.y;
        }
        center.x /= d_points.size();
        center.y /= d_points.size();

        return center;
    }

    // Calculate the circumference of the structure
    double getCircumference()
    {
        // Step 1: Calculate the center of the circle
        LagPoint center = getStructureCenter();

        // Step 2: Calculate the angle of each point relative to the center
        for (auto& p : d_points) 
        {
            p.angle = std::atan2(p.y - center.y, p.x - center.x);
        }

        // Step 3: Sort the points by their angle
        std::sort(d_points.begin(), d_points.end(), [](const LagPoint& a, const LagPoint& b) { return a.angle < b.angle; });

        // Step 4: Calculate the circumference
        double circumference = 0.0;
        for (size_t i = 0; i < d_points.size(); ++i) 
        {
            circumference += distance(d_points[i], d_points[(i + 1) % d_points.size()]);
        }

        return circumference;
    }

    // Calculate the area of the polygon
    double getArea()
    {
        // Ensure the points are sorted counterclockwise
        LagPoint center = getStructureCenter();
        for (auto& p : d_points) 
        {
            p.angle = std::atan2(p.y - center.y, p.x - center.x);
        }
        std::sort(d_points.begin(), d_points.end(), [](const LagPoint& a, const LagPoint& b) { return a.angle < b.angle; });

        // Apply the shoelace formula
        double area = 0.0;
        for (size_t i = 0; i < d_points.size(); ++i) 
        {
            const LagPoint& p1 = d_points[i];
            const LagPoint& p2 = d_points[(i + 1) % d_points.size()];
            area += (p1.x * p2.y - p1.y * p2.x);
        }

        return std::abs(area) / 2.0;
    }

    std::vector<LagPoint> getPositions() const
    {
        return d_points;
    }

    void outputTracerPositions() const
    {
        for(const auto& pt : d_points)
        {
            d_output_file << d_current_time << " " << pt.x << " " << pt.y << std::endl;
        }
    }

private:
    double d_current_time;
    std::string d_structure_name;
    std::ofstream& d_output_file; // Use reference to avoid unnecessary copying
    std::vector<LagPoint> d_points;
    IBTK::LDataManager* d_data_manager;
};
