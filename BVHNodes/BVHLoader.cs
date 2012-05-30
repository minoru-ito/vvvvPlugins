//  Loading BVH file and create Skeleton
//
//  inspired:
//  http://perfume-dev.github.com/

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Globalization;

using VVVV.Utils.VMath;
using VVVV.SkeletonInterfaces;

namespace VVVV.Nodes
{
    public class BVHLoader
    {
        //Skeleton from BVH
        private Skeleton FSkeleton;

        //list of motion data each frames
        private List<List<float>> FFrames;

        //total frame counts
        private int FFrameCount;

        //seconds per frame
        private float FFrameTime;

        //total channel counts of Skeleton
        private int FChannelCountOfSkeleton;

        //current frame
        private int FCurrentFrame;

        //joint counter
        private int FJointCounter;

        //public string debugString;

        //constructor
        public BVHLoader()
        {
            FSkeleton = new Skeleton();
            
            FFrames = new List<List<float>>();

            FChannelCountOfSkeleton = 0;
            FJointCounter = 0;
        }


        public Skeleton Skeleton
        {
            get
            {
                return FSkeleton;
            }
        }

        public int FrameCount
        {
            get
            {
                return FFrameCount;
            }
        }

        public float FrameTime
        {
            get
            {
                return FFrameTime;
            }
        }


        public void Unload()
        {
            //clear Skeleton
            FSkeleton.ClearAll();

            //reset frame data
            FFrames.Clear();

            //reset channel count
            FChannelCountOfSkeleton = 0;

            //reset JointCounter
            FJointCounter = 0;

            //reset current frame
            FCurrentFrame = 0;
        }
        
        //Loading BVH file
        public bool Load(string Filename)
        {
            string data = "";

            this.Unload();

            using (StreamReader reader = new StreamReader(Filename))
            {
                data = reader.ReadToEnd();
            }

            //get index of "HIERARCHY" and "MOTION"
            int indexHIERARCHY = data.IndexOf("HIERARCHY");
            int indexMOTION = data.IndexOf("MOTION");

            //quit if not found
            if (indexHIERARCHY < 0 || indexMOTION < 0)
            {
                //bad format

                return false;
            }

            //parse HIERARCHY
            ParseHierarchy(data.Substring(indexHIERARCHY, indexMOTION));

            //parse MOTION
            ParseMotion(data.Substring(indexMOTION));
            
            //move to 1st frame
            UpdateSkeleton(0);

            return true;
        }

        //parse HIERARCHY
        private void ParseHierarchy(string data)
        {
            List<string> tokens = new List<string>();
            string token = "";

            //check every characters of input data
            for (int i = 0; i < data.Length; i++)
            {
                char c = data[i];

                if (Char.IsWhiteSpace(c))
                {
                    //add token to List
                    if (token.Length > 0) tokens.Add(token);

                    //reset token
                    token = "";
                }
                else
                {
                    //create token
                    token += c.ToString();
                }
            }

            int index = 0;
            while (index < tokens.Count)
            {
                if (tokens[index++] == "ROOT")
                {
                    //start parse JOINT from ROOT
                    ParseJoint(ref index, ref tokens, null);
                }
            }
        }

        //parse JOINT
        private void ParseJoint(ref int index, ref List<string> tokens, BVHJoint parent)
        {
            //NOTE: can't assign same joint name in vvvv Skeleton!
            string name = tokens[index++];
            string tmpName;
            int num = 2;

            //set name without number as temp
            tmpName = name;

            //rename while enable
            while (FSkeleton.JointTable.ContainsKey(name))
            {
                name = tmpName + num.ToString();
                num++;
            }

            //create BVHJoint
            BVHJoint joint = new BVHJoint(FJointCounter++, name, parent);

            if (parent != null)
            {
                FSkeleton.InsertJoint(parent.Name, joint);
            }
            else
            {
                //"parent == null" means ROOT

                FSkeleton.InsertJoint("", joint);

                FSkeleton.Root = joint;

                FSkeleton.BuildJointTable();    //what's happen here?
            }

            while (index < tokens.Count)
            {
                //get token
                string token = tokens[index++];

                if (token == "OFFSET")
                {
                    Vector3D translate = new Vector3D();

                    //set offset(=initial) position
                    
                    //This is not working with some culture settings
                    //translate.x = double.Parse(tokens[index++]);
                    //translate.y = double.Parse(tokens[index++]);
                    //translate.z = double.Parse(tokens[index++]);// *-1;   //Right Hand to Left Hand
                    
                    //to solve that, use InvariantInfo
                    translate.x = double.Parse(tokens[index++], NumberFormatInfo.InvariantInfo);
                    translate.y = double.Parse(tokens[index++], NumberFormatInfo.InvariantInfo);
                    translate.z = double.Parse(tokens[index++], NumberFormatInfo.InvariantInfo);
                    

                    //set BaseTransform
                    joint.BaseTransform = VMath.Translate(translate);
                    //joint.AnimationTransform = VMath.Translate(translate);
                }
                else if (token == "CHANNELS")
                {
                    //get channel count from token
                    //int channelCount = int.Parse(tokens[index++]);
                    int channelCount = int.Parse(tokens[index++], NumberFormatInfo.InvariantInfo);

                    //set channel count
                    joint.SetChannelCount(channelCount);

                    //update total count of Skeleton's channels
                    FChannelCountOfSkeleton += channelCount;
                    
                    //processing each channel
                    for (int i = 0; i < channelCount; i++)
                    {
                        //to lower case
                        string channel = tokens[index++].ToLower();

                        //detect axis and tranform type by first two char
                        char axis = channel[0];
                        char trans = channel[1];
                        switch (trans)
                        {
                            case 'p':   //position
                                switch (axis)
                                {
                                    case 'x': joint.SetChannel(i, BVHChannel.X_POSITION); break;
                                    case 'y': joint.SetChannel(i, BVHChannel.Y_POSITION); break;
                                    case 'z': joint.SetChannel(i, BVHChannel.Z_POSITION); break;
                                }
                                break;
                            case 'r':   //rotation
                                switch (axis)
                                {
                                    case 'x': joint.SetChannel(i, BVHChannel.X_ROTATION); break;
                                    case 'y': joint.SetChannel(i, BVHChannel.Y_ROTATION); break;
                                    case 'z': joint.SetChannel(i, BVHChannel.Z_ROTATION); break;
                                }
                                break;
                        }
                    }
                }
                else if (token == "JOINT")
                {
                    //recusive call
                    ParseJoint(ref index, ref tokens, joint);
                }
                else if (token == "End")
                {
                    //recusive call
                    ParseJoint(ref index, ref tokens, joint);
                }
                else if (token == "}")
                {
                    //go next
                    break;
                }
            }
        }

        //parse MOTION
        private void ParseMotion(string data)
        {
            //separate per lines
            string[] lines = data.Split('\n');

            int index = 0;

            //processing per line(=motion parameter)
            while (index < lines.Length)
            {
                //get one line
                string line = lines[index];

                //do nothing if blank line
                if (line == "")
                {
                    index++;
                    continue;
                }

                if (line.IndexOf("MOTION") != -1)
                {
                    //MOTION line
                    
                    //do nothing.
                }
                else if (line.IndexOf("Frames:") != -1)
                {
                    //Frames: line

                    //separate with ':' and get second value(= frame count)
                    //FFrameCount = int.Parse(line.Split(':')[1]);
                    FFrameCount = int.Parse(line.Split(':')[1], NumberFormatInfo.InvariantInfo);
                }
                else if (line.IndexOf("Frame Time:") != -1)
                {
                    //Frame Time: line

                    //separate with ':' and get second value(= frame time)
                    //FFrameTime = float.Parse(line.Split(':')[1]);
                    FFrameTime = float.Parse(line.Split(':')[1], NumberFormatInfo.InvariantInfo);
                }
                else
                {
                    //escape loop

                    break;
                }

                index++;
            }

            //processing per line(= motion data)
            while (index < lines.Length)
            {
                //get one line
                string line = lines[index];

                //create array of empty strings
                char[] splitters = String.Empty.ToCharArray();

                //separate with empty strings, without empty
                string[] channels = line.Split(splitters, StringSplitOptions.RemoveEmptyEntries);

                if (channels.Length != FChannelCountOfSkeleton)
                {
                    //not match number of CHANNEL(=EOF?)
                    return;
                }

                //create frameData as List
                List<float> frameData = new List<float>();
                for (int i = 0; i < channels.Length; i++)
                {
                    //frameData.Add(float.Parse(channels[i]));
                    frameData.Add(float.Parse(channels[i], NumberFormatInfo.InvariantInfo));
                }

                //add frame data
                FFrames.Add(frameData);

                index++;
            }
        }

        //update JOINT
        private void UpdateJoint(ref int channelIndex, int frameIndex, BVHJoint joint)
        {
            List<float> frame;
            Vector3D translate, rotate;

            //get MOTION data of frameIndex
            frame = FFrames[frameIndex];

            //init transform
            translate = new Vector3D(0.0);
            rotate = new Vector3D(0.0);

            //set transform channel count times
            for (int i = 0; i < joint.GetChannelCount(); i++)
            {
                //get one value from MOTION data
                float f = frame[channelIndex++];

                //get target channel
                BVHChannel ch = joint.Channels[i];

                //set value to channel
                if (ch == BVHChannel.X_POSITION)
                {
                    translate.x = f;
                }
                else if (ch == BVHChannel.Y_POSITION)
                {
                    translate.y = f;
                }
                else if (ch == BVHChannel.Z_POSITION)
                {
                    translate.z = f;// -f;   //Right Hand to Left Hand
                }
                else if (ch == BVHChannel.X_ROTATION)
                {
                    //Degrees into Radians
                    rotate.x = f * VMath.DegToRad;
                }
                else if (ch == BVHChannel.Y_ROTATION)
                {
                    //Degrees into Radians
                    rotate.y = f * VMath.DegToRad;
                }
                else if (ch == BVHChannel.Z_ROTATION)
                {
                    //Degrees into Radians
                    rotate.z = f * VMath.DegToRad;
                }
            }

            //update AnimationTransform
            joint.AnimationTransform = VMath.Transform(translate, new Vector3D(1.0), rotate);
            
            //recusive call all children
            for (int i = 0; i < joint.Children.Count; i++)
            {
                UpdateJoint(ref channelIndex, frameIndex, (BVHJoint)joint.Children[i]);
            }
        }

        //update Skeleton
        public void UpdateSkeleton(int frameIndex)
        {
            //channelIndex always start from 0
            int channelIndex = 0;

            //update JOINT with target frame from ROOT
            UpdateJoint(ref channelIndex, frameIndex, (BVHJoint)FSkeleton.Root);

            //update current frame
            FCurrentFrame = frameIndex;
        }
    }
}
