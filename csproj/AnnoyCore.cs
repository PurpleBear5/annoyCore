using System;
using System.IO;
using System.Numerics;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics;

namespace Annoy
{
    public class AnnoyCore : IDisposable
    {
        private const string dllpath = "Source\\annoymodulecs.dll";


        [DllImport(dllpath, EntryPoint = "Init")]
        private static extern IntPtr Init(int f, int distType, string indexPath);

        [DllImport(dllpath, EntryPoint = "AddItem")]
        private static extern void AddItem(IntPtr ptr, int i, [In, Out] float[] vector);

        [DllImport(dllpath, EntryPoint = "BuildAndSave")]
        private static extern void BuildAndSave(IntPtr ptr, string path, int trees = 20);

        [DllImport(dllpath, EntryPoint = "NNS_Search")]
        private static extern void NNS_Search(IntPtr ptr, [In, Out] float[] vector, int items, [In, Out] int[] res, [In, Out] float[] distances);

        [DllImport(dllpath, EntryPoint = "NNS_SearchByItem")]
        private static extern void NNS_SearchByItem(IntPtr ptr, int item, int items, [In, Out] int[] res, [In, Out] float[] distances);

        [DllImport(dllpath, EntryPoint = "GetItemVector")]
        private static extern void GetItemVector(IntPtr ptr, int item, [In, Out] float[] vector);

        [DllImport(dllpath, EntryPoint = "GetDistance")]
        private static extern void GetDistance(IntPtr ptr, int itemA, int itemB, ref float distance);

        [DllImport(dllpath, CallingConvention = CallingConvention.Cdecl)]
        private static extern int ReleaseMemory(IntPtr ptr);

        private int _f = 0;
        private IntPtr _index;
        private bool _isBuilded = false;

        /// <summary>
        /// You need remember <param name="f"> and <param name="distance"> to load index next time
        /// </summary>
        /// <param name="f">Dimensions in index vector</param>
        /// <param name="distance">Index distance type</param>
        /// <param name="indexPath">Index file. Set NULL if you creating new index</param>
        public AnnoyCore(int f, AnnoyDistance distance, string indexPath = null)
        {
            _index = Init(f, (int)distance, indexPath);
            _f = f;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="i">Index of item in index</param>
        /// <param name="vector">Expected vector size == f</param>
        public void AddItem(int i, float[] vector)
        {
            if (_f != vector.Length)
                throw new ArgumentException($"Vector size: {vector.Length}. Expected vector size == {_f}");

            if (_isBuilded)
                throw new Exception("Can't add item in builded index");

            AddItem(_index, i, vector);
        }

        /// <summary>
        /// Build and save index
        /// After build you can't add new item in index
        /// Load index to use search
        /// </summary>
        /// <param name="path"></param>
        /// <param name="trees"></param>
        public void BuildAndSave(string path, int trees = 20)
        {
            BuildAndSave(_index, path, trees);
        }

        /// <summary>
        /// Find the nearest vectors by <param name="vector">
        /// Result can be very different by used distance
        /// </summary>
        /// <param name="vector"></param>
        /// <param name="items">How many items will be selected</param>
        /// <returns></returns>
        public SearchResult Search([In, Out] float[] vector, int items)
        {
            int[] i = new int[items];
            float[] d = new float[items];

            NNS_Search(_index, vector, items, i, d);

            return new SearchResult()
            {
                Items = i,
                Distances = d
            };
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="item">Index item</param>
        /// <param name="items">How many items will be selected</param>
        /// <returns></returns>
        public SearchResult SearchByItem(int item, int items)
        {
            int[] i = new int[items];
            float[] d = new float[items];

            NNS_SearchByItem(_index, item, items, i, d);

            return new SearchResult()
            {
                Items = i,
                Distances = d
            };
        }

        public float[] GetItemVector(int item)
        {
            float[] v = new float[_f];
            GetItemVector(_index, item, v);

            return v;
        }

        public float GetDistance(int itemA, int itemB)
        {
            float d = 0;
            GetDistance(_index, itemA, itemB, ref d);

            return d;
        }

        public void Dispose()
        {
            ReleaseMemory(_index);
        }
    }
}
