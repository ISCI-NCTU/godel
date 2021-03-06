/*
  Copyright May 7, 2014 Southwest Research Institute

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/
#ifndef SURFACE_BLENDING_SERVICE_H
#define SURFACE_BLENDING_SERVICE_H

#include <godel_surface_detection/scan/robot_scan.h>
#include <godel_surface_detection/detection/surface_detection.h>
#include <godel_surface_detection/interactive/interactive_surface_server.h>

#include <godel_msgs/SurfaceDetection.h>
#include <godel_msgs/SelectSurface.h>
#include <godel_msgs/SelectedSurfacesChanged.h>
#include <godel_msgs/ProcessPlanning.h>
#include <godel_msgs/SurfaceBlendingParameters.h>
#include <godel_msgs/GetAvailableMotionPlans.h>
#include <godel_msgs/SelectMotionPlan.h>
#include <godel_msgs/LoadSaveMotionPlan.h>
#include <godel_msgs/ProcessPlan.h>
#include <godel_msgs/RenameSurface.h>
#include <godel_msgs/ScanPlanParameters.h>

#include <godel_msgs/BlendProcessPlanning.h>
#include <godel_msgs/KeyenceProcessPlanning.h>

#include <godel_process_path_generation/VisualizeBlendingPlan.h>
#include <godel_process_path_generation/mesh_importer.h>
#include <godel_process_path_generation/utils.h>
#include <godel_process_path_generation/polygon_utils.h>

#include <godel_surface_detection/scan/profilimeter_scan.h>

#include <godel_surface_detection/services/trajectory_library.h>

#include <pcl/console/parse.h>
#include <rosbag/bag.h>

//  marker namespaces
const static std::string BOUNDARY_NAMESPACE = "process_boundary";
const static std::string PATH_NAMESPACE = "process_path";
const static std::string TOOL_NAMESPACE = "process_tool";

struct ProcessPathDetails
{
  visualization_msgs::MarkerArray process_boundaries_;
  visualization_msgs::MarkerArray process_paths_;
  visualization_msgs::MarkerArray tool_parts_;
  visualization_msgs::MarkerArray scan_paths_; // profilimeter
};

/**
 * Associates a name with a visual msgs marker which contains a pose and sequence of points defining
 * a path
 */
struct ProcessPathResult
{
  typedef std::pair<std::string, visualization_msgs::Marker> value_type;
  std::vector<value_type> paths;
};
/**
 * Associates a name with a joint trajectory
 */
struct ProcessPlanResult
{
  typedef std::pair<std::string, godel_msgs::ProcessPlan> value_type;
  std::vector<value_type> plans;
};

class SurfaceBlendingService
{
public:
  SurfaceBlendingService();

  bool init();
  void run();

private:
  bool load_blend_parameters(const std::string& filename);

  void save_blend_parameters(const std::string& filename);

  bool load_scan_parameters(const std::string& filename);

  void save_scan_parameters(const std::string& filename);

  void publish_selected_surfaces_changed();

  bool run_robot_scan(visualization_msgs::MarkerArray& surfaces);

  bool find_surfaces(visualization_msgs::MarkerArray& surfaces);

  void remove_previous_process_plan();

  /**
   * The following path generation and planning methods are defined in
   * src/blending_service_path_generation.cpp
   */
  bool generate_process_plan(godel_process_path_generation::VisualizeBlendingPlan& process_plan);

  bool animate_tool_path();

  void tool_animation_timer_callback();

  visualization_msgs::MarkerArray create_tool_markers(const geometry_msgs::Point& pos,
                                                      const geometry_msgs::Pose& pose,
                                                      std::string frame_id);

  // Service callbacks, these components drive this class by signalling events
  // from the user
  bool surface_detection_server_callback(godel_msgs::SurfaceDetection::Request& req,
                                         godel_msgs::SurfaceDetection::Response& res);

  bool select_surface_server_callback(godel_msgs::SelectSurface::Request& req,
                                      godel_msgs::SelectSurface::Response& res);

  bool process_path_server_callback(godel_msgs::ProcessPlanning::Request& req,
                                    godel_msgs::ProcessPlanning::Response& res);

  bool
  surface_blend_parameters_server_callback(godel_msgs::SurfaceBlendingParameters::Request& req,
                                           godel_msgs::SurfaceBlendingParameters::Response& res);

  // Reads from the surface selection server and generates blend/scan paths for each
  godel_surface_detection::TrajectoryLibrary
  generateMotionLibrary(const godel_msgs::BlendingPlanParameters& blend_params,
                        const godel_msgs::ScanPlanParameters& scan_params);

  bool requestBlendPath(const godel_process_path::PolygonBoundaryCollection& boundaries,
                        const geometry_msgs::Pose& boundary_pose,
                        const godel_msgs::BlendingPlanParameters& params,
                        visualization_msgs::Marker& path);

  bool requestScanPath(const godel_process_path::PolygonBoundaryCollection& boundaries,
                       const geometry_msgs::Pose& boundary_pose,
                       const godel_msgs::ScanPlanParameters& params,
                       visualization_msgs::Marker& path);

  ProcessPathResult generateProcessPath(const std::string& name, const pcl::PolygonMesh& mesh,
                                        const godel_msgs::BlendingPlanParameters& params,
                                        const godel_msgs::ScanPlanParameters& scan_params);

  ProcessPlanResult generateProcessPlan(const std::string& name,
                                        const visualization_msgs::Marker& path,
                                        const godel_msgs::BlendingPlanParameters& params,
                                        const godel_msgs::ScanPlanParameters& scan_params);

  bool selectMotionPlanCallback(godel_msgs::SelectMotionPlan::Request& req,
                                godel_msgs::SelectMotionPlan::Response& res);

  bool getMotionPlansCallback(godel_msgs::GetAvailableMotionPlans::Request& req,
                              godel_msgs::GetAvailableMotionPlans::Response& res);

  bool loadSaveMotionPlanCallback(godel_msgs::LoadSaveMotionPlan::Request& req,
                                  godel_msgs::LoadSaveMotionPlan::Response& res);

  bool renameSurfaceCallback(godel_msgs::RenameSurface::Request& req,
                             godel_msgs::RenameSurface::Response& res);

  void visualizePaths();

  // Services offered by this class
  ros::ServiceServer surface_detect_server_;
  ros::ServiceServer select_surface_server_;
  ros::ServiceServer process_path_server_;
  ros::ServiceServer surf_blend_parameters_server_;

  ros::ServiceServer get_motion_plans_server_;
  ros::ServiceServer select_motion_plan_server_;
  ros::ServiceServer load_save_motion_plan_server_;
  ros::ServiceServer rename_suface_server_;

  // Services subscribed to by this class
  ros::ServiceClient visualize_process_path_client_;
  ros::ServiceClient trajectory_planner_client_;

  ros::ServiceClient blend_planning_client_;
  ros::ServiceClient keyence_planning_client_;

  // Process Execution service clients
  ros::ServiceClient blend_process_client_;
  ros::ServiceClient scan_process_client_;

  // Current state publishers
  ros::Publisher selected_surf_changed_pub_;
  ros::Publisher point_cloud_pub_;
  ros::Publisher tool_path_markers_pub_;
  // Timers
  bool stop_tool_animation_;

  // robot scan instance
  godel_surface_detection::scan::RobotScan robot_scan_;
  // surface detection instance
  godel_surface_detection::detection::SurfaceDetection surface_detection_;
  // marker server instance
  godel_surface_detection::interactive::InteractiveSurfaceServer surface_server_;
  // mesh importer for generating surface boundaries
  godel_process_path::MeshImporter mesh_importer_;

  // parameters
  godel_msgs::RobotScanParameters default_robot_scan_params__;
  godel_msgs::SurfaceDetectionParameters default_surf_detection_params_;
  godel_msgs::BlendingPlanParameters default_blending_plan_params_;
  godel_msgs::ScanPlanParameters default_scan_params_;
  godel_msgs::ScanPlanParameters scan_plan_params_;
  godel_msgs::BlendingPlanParameters blending_plan_params_;

  // results
  godel_msgs::SurfaceDetection::Response latest_surface_detection_results_;
  ProcessPathDetails process_path_results_;
  std::vector<std::vector<ros::Duration> >
      duration_results_; // returned by visualize plan service, needed by trajectory planner

  // parameters
  bool publish_region_point_cloud_;

  // msgs
  sensor_msgs::PointCloud2 region_cloud_msg_;

  godel_surface_detection::TrajectoryLibrary trajectory_library_;
  int marker_counter_;

  // Parameter loading and saving
  std::string param_cache_prefix_;
};

#endif // surface blending services
