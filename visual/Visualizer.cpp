//
// Created by wrede on 04.05.16.
//

#include "Visualizer.h"
#include "../util/Logger.h"
#include "../core/ObjectDataAngular.h"
#include "../util/FileIO.h"

namespace visual
{
    void Visualizer::Display(core::DetectionSequence& sequence,
                             std::string image_folder,
                             std::string title, size_t first_frame,
                             int play_fps)
    {
        size_t current_frame = first_frame;

        // Load images
        std::vector<std::string> image_files;
        util::FileIO::ListFiles(image_folder, image_files);

        if (image_files.size() != sequence.GetFrameCount())
        {
            perror("Image count and frame count don't match\n");
        }

        // Create window
        cv::namedWindow(title, CV_WINDOW_AUTOSIZE);

        // Display frames and data
        int target_delay = 1000 / play_fps;
        int last_frame_time = GetTime();
        int current_delay, current_frame_time;
        bool play = false;
        while (true)
        {
            // Display image
            cv::Mat image = cv::imread(image_folder + "/"
                                       + image_files[current_frame],
                                       1);


            for (size_t i = 0; i < sequence.GetObjectCount(current_frame); ++i)
            {
                core::ObjectDataPtr obj = sequence.GetObject(current_frame, i);

                // BGR
                cv::Scalar color(0,
                                 (1.0 - obj->GetDetectionScore()) * 255,
                                 obj->GetDetectionScore() * 255);

                obj->Visualize(image, color);
            }

            cv::imshow(title, image);

            // Receive key input
            int key;

            if (play)
            {
                current_frame_time = GetTime();
                current_delay = last_frame_time - current_frame_time;
                if (current_delay < target_delay)
                {
                    key = cv::waitKey(target_delay - current_delay);
                }
                else
                {
                    key = 0;
                }
                last_frame_time = GetTime();
            }
            else
            {
                key = cv::waitKey(0);
            }

            // Process key input
            if (key == 1048678) // F
            {
                play = !play;
            }
            else if (key == 1048603) // ESC
            {
                break;
            }

            if (play || key == 1048676) // D
            {
                if (current_frame < image_files.size() - 1)
                {
                    current_frame++;
                }
                else
                {
                    current_frame = image_files.size() - 1;
                    play = false;
                }
            }
            else if (key == 1048673) // A
            {
                if (current_frame > 0)
                {
                    current_frame--;
                }
            }
        }
    }

    int Visualizer::GetTime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Visualizer::Display(std::vector<core::TrackletPtr>& tracks,
                             std::string image_folder, std::string title,
                             size_t first_frame, int play_fps)
    {
        size_t current_frame = first_frame;

        // Load images
        std::vector<std::string> image_files;
        util::FileIO::ListFiles(image_folder, image_files);

        // Create window
        cv::namedWindow(title, CV_WINDOW_AUTOSIZE);

        // Generate a random color for each individual track
        std::vector<cv::Scalar> colors;
        std::random_device rd;
        std::mt19937 gen(rd());
        for (size_t i = 0; i < tracks.size(); ++i)
        {
            // BGR
            cv::Scalar color(std::generate_canonical<double, 10>(gen) * 255,
                             std::generate_canonical<double, 10>(gen) * 255,
                             std::generate_canonical<double, 10>(gen) * 255);
            colors.push_back(color);
        }

        // Display frames and data
        int target_delay = 1000 / play_fps;
        int last_frame_time = GetTime();
        int current_delay, current_frame_time;
        bool play = false;
        while (true)
        {
            // Display image
            cv::Mat image = cv::imread(image_folder + "/"
                                       + image_files[current_frame],
                                       1);

            util::Logger::LogDebug("visualize frame " + std::to_string(current_frame));
            for (size_t i = 0; i < tracks.size(); ++i)
            {
                tracks[i]->Visualize(image, colors[i], current_frame, 1, 1);
            }

            cv::imshow(title, image);

            // Get key input
            int key;
            if (play)
            {
                current_frame_time = GetTime();
                current_delay = last_frame_time - current_frame_time;
                if (current_delay < target_delay)
                {
                    key = cv::waitKey(target_delay - current_delay);
                }
                else
                {
                    key = 0;
                }
                last_frame_time = GetTime();
            }
            else
            {
                key = cv::waitKey(0);
            }

            // Process key input
            if (key == 1048678) // F
            {
                play = !play;
            }
            else if (key == 1048603) // ESC
            {
                break;
            }

            if (play || key == 1048676) // D
            {
                if (current_frame < image_files.size() - 1)
                {
                    current_frame++;
                }
                else
                {
                    current_frame = image_files.size() - 1;
                    play = false;
                }
            }
            else if (key == 1048673) // A
            {
                if (current_frame > 0)
                {
                    current_frame--;
                }
            }
        }
    }
}