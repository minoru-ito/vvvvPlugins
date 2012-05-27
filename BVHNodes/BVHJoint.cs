using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using VVVV.Utils.VMath;
using VVVV.SkeletonInterfaces;

namespace VVVV.Nodes
{
    public enum BVHChannel
    {
        X_POSITION,
        Y_POSITION,
        Z_POSITION,
        X_ROTATION,
        Y_ROTATION,
        Z_ROTATION
    }

    public class BVHJoint : IJoint
    {
        //IJoint Members
        private string FName;
        private int FId;
        private IJoint FParent;
        private List<IJoint> FChildren;
        private List<Vector2D> FConstraints;
        private Matrix4x4 FBaseTransform;
        private Matrix4x4 FAnimationTransform;
        private Matrix4x4 FCombinedTransform;
        private Vector3D FTranslation;
        private Vector3D FRotation;
        private Vector3D FScale;

        //BVHJoint Members
        private BVHChannel[] FChannels;

        //Constructor
        public BVHJoint(int id, string name, IJoint parent)
		{
			FId = id;
			FName = name;
            FParent = parent;
			FChildren = new List<IJoint>();
			
			FBaseTransform = VMath.IdentityMatrix;
			FAnimationTransform = VMath.IdentityMatrix;
			FConstraints = new List<Vector2D>();
			FConstraints.Add(new Vector2D(-1.0, 1.0));
			FConstraints.Add(new Vector2D(-1.0, 1.0));
			FConstraints.Add(new Vector2D(-1.0, 1.0));
		}

        //---------------------------------IJoint

        public string Name
        {
            set
            {
                FName = value;
            }
            get
            {
                return FName;
            }
        }

        public int Id
        {
            set
            {
                FId = value;
            }
            get
            {
                return FId;
            }
        }

        public Matrix4x4 BaseTransform
        {
            set
            {
                FBaseTransform = value;
            }
            get
            {
                return FBaseTransform;
            }
        }

        public Matrix4x4 AnimationTransform
        {
            set
            {
                FAnimationTransform = value;
            }
            get
            {
                return FAnimationTransform;
            }
        }

        public IJoint Parent
        {
            get
            {
                return FParent;
            }

            set
            {
                FParent = value;
            }
        }

        public List<IJoint> Children
        {
            get
            {
                return FChildren;
            }
        }

        public Vector3D Rotation
        {
            get
            {
                return FRotation;
            }
        }

        public Vector3D Translation
        {
            get
            {
                return FTranslation;
            }
        }

        public Vector3D Scale
        {
            get
            {
                return FScale;
            }
        }

        public List<Vector2D> Constraints
        {
            get
            {
                return FConstraints;
            }
            set
            {
                FConstraints = value;
            }
        }

        public Matrix4x4 CombinedTransform
        {
            get
            {
                //is this good?
                CalculateCombinedTransforms();

                return FCombinedTransform;
            }
        }

        public void CalculateCombinedTransforms()
        {
            //UpdateCachedValues();

            AnimationTransform.Decompose(out FScale, out FRotation, out FTranslation);

            if (Parent != null)
                FCombinedTransform = AnimationTransform * BaseTransform * Parent.CombinedTransform;
            else
                FCombinedTransform = AnimationTransform * BaseTransform;
        }

        public void AddChild(IJoint joint)
        {
            joint.Parent = this;
            Children.Add(joint);
        }

        public void ClearAll()
        {
            Children.Clear();
        }

        public IJoint DeepCopy()
        {
            BVHJoint copy = new BVHJoint(Id, Name, Parent);
            copy.BaseTransform = new Matrix4x4(BaseTransform);
            copy.AnimationTransform = new Matrix4x4(AnimationTransform);

            foreach (IJoint child in Children)
                copy.AddChild(child.DeepCopy());

            for (int i = 0; i < 3; i++)
                copy.Constraints[i] = new Vector2D(Constraints[i]);

            return copy;
        }

        /*
        private void UpdateCachedValues()
        {
            if (IsDirty())
            {
                AnimationTransform.Decompose(out FCachedScale, out FCachedRotation, out FCachedTranslation);
                if (Parent != null)
                    FCachedCombinedTransform = AnimationTransform * BaseTransform * Parent.CombinedTransform;
                else
                    FCachedCombinedTransform = AnimationTransform * BaseTransform;
                FDirty = false;
            }
        }
        */

        //---------------------------------BVHJoint

        public void SetChannelCount(int count)
        {
            FChannels = new BVHChannel[count];
        }

        public int GetChannelCount()
        {
            if (FChannels != null)
            {
                return FChannels.Length;
            }
            else
            {
                return 0;
            }
        }

        public BVHChannel[] Channels
        {
            get
            {
                return FChannels;
            }
        }

        public void SetChannel(int index, BVHChannel channel)
        {
            FChannels[index] = channel;
        }

    }
}
