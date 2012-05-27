//  create VVVV.Skeleton with motion data from BVH file

#region usings
using System;
using System.ComponentModel.Composition;

using VVVV.PluginInterfaces.V1;
using VVVV.PluginInterfaces.V2;
using VVVV.SkeletonInterfaces;
using VVVV.Utils.VMath;

using VVVV.Core.Logging;
#endregion usings

namespace VVVV.Nodes
{
    #region PluginInfo
    [PluginInfo(Name = "BVH", Category = "Skeleton BVH", Help = "output skeleton with animation from bvh file", Author = "mino")]
    #endregion PluginInfo
    public class BVHNode : IPluginEvaluate
    {
        #region fields & pins
        [Input("Filename", IsSingle = true, StringType = StringType.Filename, FileMask = "BVH Files (*.bvh)|*.bvh")]
        IDiffSpread<string> FFileNameIn;

        [Input("Frame", IsSingle = true, MinValue = 0)]
        IDiffSpread<int> FFrameIn;

        [Output("Frames")]
        ISpread<int> FFramesOut;

        [Output("Frame Time")]
        ISpread<float> FFrameTimeOut;

        private INodeOut FSkeletonOutput;

        [Import()]
        ILogger FLogger;

        private Skeleton FSkeleton;

        private BVHLoader FBVHLoader;

        #endregion fields & pins

        [ImportingConstructor]
        public BVHNode(IPluginHost host)
        {
            //Create Skeleton Output
            System.Guid[] guids = new System.Guid[1];
            guids[0] = new Guid("AB312E34-8025-40F2-8241-1958793F3D39");
            
            host.CreateNodeOutput("Skeleton", TSliceMode.Single, TPinVisibility.True, out FSkeletonOutput);
            FSkeletonOutput.SetSubType(guids, "Skeleton");


            //init BVHLoader
            FBVHLoader = new BVHLoader();
        }

        //called when data for any output pin is requested
        public void Evaluate(int SpreadMax)
        {
            if (FFileNameIn.IsChanged)
            {
                if (FFileNameIn[0].EndsWith(".bvh"))
                {
                    FSkeleton = null;

                    //load BVH file
                    if (FBVHLoader.Load(FFileNameIn[0]))
                    {
                        //set frame count and second per frame
                        FFramesOut.SliceCount = 1;
                        FFrameTimeOut.SliceCount = 1;
                        FFramesOut[0] = FBVHLoader.FrameCount;
                        FFrameTimeOut[0] = FBVHLoader.FrameTime;

                        //set Skeleton Output
                        FSkeleton = FBVHLoader.Skeleton;
                        FSkeletonOutput.SliceCount = 1;
                        FSkeletonOutput.SetInterface(FSkeleton);
                        FSkeletonOutput.MarkPinAsChanged();
                    }
                    else
                    {
                        //reset frame count and second per frame
                        FFramesOut.SliceCount = 0;
                        FFrameTimeOut.SliceCount = 0;

                        //reset Skeleton Output
                        FSkeletonOutput.SliceCount = 0;
                        FSkeletonOutput.MarkPinAsChanged();

                        FLogger.Log(LogType.Debug, "BVH load error");
                    }
                    
                }
                else
                {
                    FSkeleton = null;

                    FBVHLoader.Unload();

                    //reset frame count and second per frame
                    FFramesOut.SliceCount = 0;
                    FFrameTimeOut.SliceCount = 0;

                    //reset Skeleton Output
                    FSkeletonOutput.SliceCount = 0;
                    FSkeletonOutput.MarkPinAsChanged();
                }
            }
 
            if (FFrameIn.IsChanged)
            {
                if (FSkeleton != null)
                {
                    //update Skeleton (be sure frame ID <= frame count)
                    FBVHLoader.UpdateSkeleton(FFrameIn[0] % FBVHLoader.FrameCount);

                    //set Skeleton Output
                    FSkeleton = FBVHLoader.Skeleton;
                    FSkeletonOutput.SetInterface(FSkeleton);
                    FSkeletonOutput.MarkPinAsChanged();
                }
            }

            //FLogger.Log(LogType.Debug, "Logging to Renderer (TTY)");
        }
    }
}
