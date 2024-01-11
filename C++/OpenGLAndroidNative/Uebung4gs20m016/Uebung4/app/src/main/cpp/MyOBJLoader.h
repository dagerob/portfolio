//
// Created by dange on 05/01/2022.
//
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <android/log.h>

namespace lder{
    // Structure: Vector2
    //
    // Description: A 2D Vector that Holds Positional Data
    struct Vector2
    {
        // Default Constructor
        Vector2()
        {
            X = 0.0f;
            Y = 0.0f;
        }
        // Variable Set Constructor
        Vector2(float X_, float Y_)
        {
            X = X_;
            Y = Y_;
        }
        // Bool Equals Operator Overload
        bool operator==(const Vector2& other) const
        {
            return (this->X == other.X && this->Y == other.Y);
        }
        // Bool Not Equals Operator Overload
        bool operator!=(const Vector2& other) const
        {
            return !(this->X == other.X && this->Y == other.Y);
        }
        // Addition Operator Overload
        Vector2 operator+(const Vector2& right) const
        {
            return Vector2(this->X + right.X, this->Y + right.Y);
        }
        // Subtraction Operator Overload
        Vector2 operator-(const Vector2& right) const
        {
            return Vector2(this->X - right.X, this->Y - right.Y);
        }
        // Float Multiplication Operator Overload
        Vector2 operator*(const float& other) const
        {
            return Vector2(this->X *other, this->Y * other);
        }

        // Positional Variables
        float X;
        float Y;
    };

    // Structure: Vector3
    //
    // Description: A 3D Vector that Holds Positional Data
    struct Vector3
    {
        // Default Constructor
        Vector3()
        {
            X = 0.0f;
            Y = 0.0f;
            Z = 0.0f;
        }
        // Variable Set Constructor
        Vector3(float X_, float Y_, float Z_)
        {
            X = X_;
            Y = Y_;
            Z = Z_;
        }
        // Bool Equals Operator Overload
        bool operator==(const Vector3& other) const
        {
            return (this->X == other.X && this->Y == other.Y && this->Z == other.Z);
        }
        // Bool Not Equals Operator Overload
        bool operator!=(const Vector3& other) const
        {
            return !(this->X == other.X && this->Y == other.Y && this->Z == other.Z);
        }
        // Addition Operator Overload
        Vector3 operator+(const Vector3& right) const
        {
            return Vector3(this->X + right.X, this->Y + right.Y, this->Z + right.Z);
        }
        // Subtraction Operator Overload
        Vector3 operator-(const Vector3& right) const
        {
            return Vector3(this->X - right.X, this->Y - right.Y, this->Z - right.Z);
        }
        // Float Multiplication Operator Overload
        Vector3 operator*(const float& other) const
        {
            return Vector3(this->X * other, this->Y * other, this->Z * other);
        }
        // Float Division Operator Overload
        Vector3 operator/(const float& other) const
        {
            return Vector3(this->X / other, this->Y / other, this->Z / other);
        }

        // Positional Variables
        float X;
        float Y;
        float Z;
    };

    namespace algorithm {
        // Split a String into a string array at a given token
        inline void split(const std::string &in,
                          std::vector<std::string> &out,
                          std::string token)
        {
            out.clear();

            std::string temp;

            for (int i = 0; i < int(in.size()); i++)
            {
                std::string test = in.substr(i, token.size());

                if (test == token)
                {
                    if (!temp.empty())
                    {
                        out.push_back(temp);
                        temp.clear();
                        i += (int)token.size() - 1;
                    }
                    else
                    {
                        out.push_back("");
                    }
                }
                else if (i + token.size() >= in.size())
                {
                    temp += in.substr(i, token.size());
                    out.push_back(temp);
                    break;
                }
                else
                {
                    temp += in[i];
                }
            }
        }

        // Get tail of string after first token and possibly following spaces
        inline std::string tail(const std::string &in)
        {
            size_t token_start = in.find_first_not_of(" \t");
            size_t space_start = in.find_first_of(" \t", token_start);
            size_t tail_start = in.find_first_not_of(" \t", space_start);
            size_t tail_end = in.find_last_not_of(" \t");
            if (tail_start != std::string::npos && tail_end != std::string::npos)
            {
                return in.substr(tail_start, tail_end - tail_start + 1);
            }
            else if (tail_start != std::string::npos)
            {
                return in.substr(tail_start);
            }
            return "";
        }

        // Get first token of string
        inline std::string firstToken(const std::string &in)
        {
            if (!in.empty())
            {
                size_t token_start = in.find_first_not_of(" \t");
                size_t token_end = in.find_first_of(" \t", token_start);
                if (token_start != std::string::npos && token_end != std::string::npos)
                {
                    return in.substr(token_start, token_end - token_start);
                }
                else if (token_start != std::string::npos)
                {
                    return in.substr(token_start);
                }
            }
            return "";
        }
    }

    class Loader{
    public:
        std::vector<Vector2> uvs;
        std::vector<Vector3> positions;
        std::vector<Vector3> normals;
        std::vector<unsigned int> indices;
        int vertsSize;
        Loader(){};
        void LoadFile(std::stringstream* file){
            // reset all containers and values
            uvs.clear();
            positions.clear();
            normals.clear();
            indices.clear();
            vertsSize = 0;

            // read line per line the file
            std::string curline;
            while (std::getline(*file, curline))
            {
//                __android_log_print(ANDROID_LOG_VERBOSE, "LOADER", "%s", algorithm::firstToken(curline).c_str());
                std::string firstToken = algorithm::firstToken(curline);
                if(firstToken == "v"){
                    std::vector<std::string> strPositions;
                    algorithm::split(algorithm::tail(curline),strPositions," ");
                    Vector3 vpos;
                    vpos.X = std::stof(strPositions[0]);
                    vpos.Y = std::stof(strPositions[1]);
                    vpos.Z = std::stof(strPositions[2]);
                    positions.push_back(vpos);
                }else if(firstToken == "vt"){
                    std::vector<std::string> strUVs;
                    algorithm::split(algorithm::tail(curline),strUVs," ");
                    Vector2 vuvs;
                    vuvs.X = std::stof(strUVs[0]);
                    vuvs.Y = -std::stof(strUVs[1]);
                    uvs.push_back(vuvs);
                }else if(firstToken == "vn"){
                    std::vector<std::string> strNormals;
                    algorithm::split(algorithm::tail(curline),strNormals," ");
                    Vector3 vnorm;
                    vnorm.X = std::stof(strNormals[0]);
                    vnorm.Y = std::stof(strNormals[1]);
                    vnorm.Z = std::stof(strNormals[2]);
                    normals.push_back(vnorm);
                }else if(firstToken == "f"){
                    std::vector<std::string> strIndexes;
                    algorithm::split(algorithm::tail(curline),strIndexes," ");
                    for(int i = 0; i < 3; i++){
                        std::vector<std::string> strNumbers;
                        algorithm::split(strIndexes[i],strNumbers,"/");
                        indices.push_back((unsigned int)std::stoi(strNumbers[0]));
                        indices.push_back((unsigned int)std::stoi(strNumbers[1]));
                        indices.push_back((unsigned int)std::stoi(strNumbers[2]));
                    }
                }
                vertsSize = positions.size();
            }
        }
    };
}