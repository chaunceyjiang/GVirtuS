/*
 * gVirtuS -- A GPGPU transparent virtualization component.
 *
 * Copyright (C) 2009-2010  The University of Napoli Parthenope at Naples.
 *
 * This file is part of gVirtuS.
 *
 * gVirtuS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * gVirtuS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gVirtuS; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Written by: Giuseppe Coviello <giuseppe.coviello@uniparthenope.it>,
 *             Department of Applied Science
 */

#include <CudaRt_internal.h>
#include "CudaRt.h"

using namespace std;

extern "C" __host__ cudaError_t CUDARTAPI cudaConfigureCall(
    dim3 gridDim, dim3 blockDim, size_t sharedMem, cudaStream_t stream) {
  CudaRtFrontend::Prepare();

    CudaRtFrontend::AddVariableForArguments(gridDim);
    CudaRtFrontend::AddVariableForArguments(blockDim);
    CudaRtFrontend::AddVariableForArguments(sharedMem);

#if CUDART_VERSION >= 3010
    CudaRtFrontend::AddDevicePointerForArguments(stream);
#else
    CudaRtFrontend::AddVariableForArguments(stream);
#endif
    CudaRtFrontend::Execute("cudaConfigureCall");
    cudaError_t cudaError = CudaRtFrontend::GetExitCode();
    return cudaError;
}

#ifndef CUDA_VERSION
#error CUDA_VERSION not defined
#endif
#if CUDA_VERSION >= 2030
extern "C" __host__ cudaError_t CUDARTAPI
cudaFuncGetAttributes(struct cudaFuncAttributes *attr, const void *func) {
  CudaRtFrontend::Prepare();
  CudaRtFrontend::AddHostPointerForArguments(attr);
  CudaRtFrontend::AddVariableForArguments((gvirtus::common::pointer_t)func);
  CudaRtFrontend::Execute("cudaFuncGetAttributes");
  if (CudaRtFrontend::Success())
    memmove(attr, CudaRtFrontend::GetOutputHostPointer<cudaFuncAttributes>(),
            sizeof(cudaFuncAttributes));
  return CudaRtFrontend::GetExitCode();
}
#endif

extern "C" __host__ cudaError_t CUDARTAPI
cudaFuncSetCacheConfig(const void *func, cudaFuncCache cacheConfig) {
  CudaRtFrontend::Prepare();
  CudaRtFrontend::AddVariableForArguments((gvirtus::common::pointer_t)func);
  CudaRtFrontend::AddVariableForArguments(cacheConfig);
  CudaRtFrontend::Execute("cudaFuncSetCacheConfig");

  return CudaRtFrontend::GetExitCode();
}

extern "C" __host__ cudaError_t CUDARTAPI cudaLaunch(const void *entry) {
#if 0
    CudaRtFrontend::AddStringForArguments(CudaUtil::MarshalHostPointer(entry));
    CudaRtFrontend::Execute("cudaLaunch");
    return CudaRtFrontend::GetExitCode();
#endif
  Buffer *launch = CudaRtFrontend::GetLaunchBuffer();
  // LAUN
  launch->Add<int>(0x4c41554e);
  launch->Add<gvirtus::common::pointer_t>((gvirtus::common::pointer_t)entry);
  CudaRtFrontend::Execute("cudaLaunch", launch);
  cudaError_t error = CudaRtFrontend::GetExitCode();
  //#ifdef DEBUG
  //    cerr << "Managing" << endl;
  //#endif
  //    CudaRtFrontend::manage();
  //#ifdef DEBUG
  //    cerr << "Managed" << endl;
  //#endif
  return error;
}

extern "C" __host__ cudaError_t CUDARTAPI cudaSetDoubleForDevice(double *d) {
  CudaRtFrontend::Prepare();
  CudaRtFrontend::AddHostPointerForArguments(d);
  CudaRtFrontend::Execute("cudaSetDoubleForDevice");
  if (CudaRtFrontend::Success())
    *d = *(CudaRtFrontend::GetOutputHostPointer<double>());
  return CudaRtFrontend::GetExitCode();
}

extern "C" __host__ cudaError_t CUDARTAPI cudaSetDoubleForHost(double *d) {
  CudaRtFrontend::Prepare();
  CudaRtFrontend::AddHostPointerForArguments(d);
  CudaRtFrontend::Execute("cudaSetDoubleForHost");
  if (CudaRtFrontend::Success())
    *d = *(CudaRtFrontend::GetOutputHostPointer<double>());
  return CudaRtFrontend::GetExitCode();
}

extern "C" __host__ cudaError_t CUDARTAPI cudaSetupArgument(const void *arg,
                                                            size_t size,
                                                            size_t offset) {
#if 0
    CudaRtFrontend::AddHostPointerForArguments(static_cast<const char *> (arg), size);
    CudaRtFrontend::AddVariableForArguments(size);
    CudaRtFrontend::AddVariableForArguments(offset);
    CudaRtFrontend::Execute("cudaSetupArgument");
    return CudaRtFrontend::GetExitCode();
#endif
  const void *pointer = *(const void **)arg;

  Buffer *launch = CudaRtFrontend::GetLaunchBuffer();
  // STAG
  if (CudaRtFrontend::isMappedMemory(*(void **)arg)) {
    gvirtus::common::mappedPointer p =
        CudaRtFrontend::getMappedPointer(*(void **)arg);
    pointer = (const void *)p.pointer;
    CudaRtFrontend::addtoManage(*(void **)arg);
    cerr << "Async device: " << p.pointer << " host: " << *(void **)arg << endl;
    cudaMemcpyAsync(p.pointer, *(void **)arg, p.size, cudaMemcpyHostToDevice,
                    NULL);
  }

  launch->Add<int>(0x53544147);
  launch->Add<char>(static_cast<char *>(const_cast<void *>((void *)&pointer)),
                    size);
  launch->Add(size);
  launch->Add(offset);
  //return cudaSuccess;
    CudaRtFrontend::Execute("cudaSetupArgument");
    return CudaRtFrontend::GetExitCode();
}

#if CUDA_VERSION >= 9000

extern "C" __host__ cudaError_t cudaLaunchKernel ( const void* func,
                                                   dim3 gridDim, dim3 blockDim,
                                                   void** args,
                                                   size_t sharedMem, cudaStream_t stream ) {
    cudaError_t cudaError = cudaSuccess;

    // A vector with the mapped pointers to be marshalled and unmarshalled
    vector<gvirtus::common::mappedPointer> mappedPointers;

    CudaRtFrontend::Prepare();
    CudaRtFrontend::AddDevicePointerForArguments(func);
    CudaRtFrontend::AddVariableForArguments(gridDim);
    CudaRtFrontend::AddVariableForArguments(blockDim);
    CudaRtFrontend::AddVariableForArguments(sharedMem);

#if CUDART_VERSION >= 3010
    CudaRtFrontend::AddDevicePointerForArguments(stream);
#else
    CudaRtFrontend::AddVariableForArguments(stream);
#endif

    std::string deviceFunc=CudaRtFrontend::getDeviceFunc(const_cast<void *>(func));
    NvInfoFunction infoFunction = CudaRtFrontend::getInfoFunc(deviceFunc);

    size_t argsPayloadSize=0;
    for (NvInfoKParam infoKParam:infoFunction.params) {
        //printf("index: %d align%x ordinal:%d offset:%d a:%x size:%d %d b:%x\n",  infoKParam.index, infoKParam.index, infoKParam.ordinal,
        //       infoKParam.offset, infoKParam.a, (infoKParam.size & 0xf8) >> 2, infoKParam.size & 0x07, infoKParam.b);
        argsPayloadSize = argsPayloadSize + ((infoKParam.size & 0xf8) >> 2);
    }

    //printf("argsSize:%d\n",argsSize);
    byte *pArgsPayload = static_cast<byte *>(malloc(argsPayloadSize));
    memset(pArgsPayload,0x00,argsPayloadSize);

    //void *argsToMarshall[infoFunction.params.size()];

    for (NvInfoKParam infoKParam:infoFunction.params) {
        byte *p=pArgsPayload+infoKParam.offset;
        //printf("%d: %x <--  %x -> %x\n",infoKParam.ordinal, p,args[infoKParam.ordinal],*(reinterpret_cast<unsigned int *>(args[infoKParam.ordinal])));


        memcpy(p,args[infoKParam.ordinal],((infoKParam.size & 0xf8) >> 2));

        /*
        if (CudaRtFrontend::isMappedMemory(*(void **)(args[infoKParam.ordinal]))) {

            void *hostPointer = *(void **)(args[infoKParam.ordinal]);

            //printf("cudaLaunchKernel: 0x%x is a mapped pointer!\n",hostPointer);
            gvirtus::common::mappedPointer mappedPointer = CudaRtFrontend::getMappedPointer(hostPointer);
            memcpy(p,&(mappedPointer.pointer),((infoKParam.size & 0xf8) >> 2));

            gvirtus::common::mappedPointer localPointer;
            localPointer.size = mappedPointer.size;
            localPointer.pointer = hostPointer;
            mappedPointers.push_back(localPointer);
        }
         */

    }
/*
    printf("-------------------\n");
    for(int i=0;i<infoFunction.params.size();i++) {
        //printf("%d: %x -> %x\n",i,args1[i],*(reinterpret_cast<unsigned int *>(args1[i])));
        printf("%d: %x\n",i,argsToMarshall[i]);
    }
    printf("-------------------\n");
*/
    //CudaRtFrontend::hexdump(pArgsPayload,argsPayloadSize);


    //printf("cudaLaunchKernel - hostFunc:%x deviceFunc:%s parameters:%d\n",func, deviceFunc.c_str(),infoFunction.params.size());

    CudaRtFrontend::AddHostPointerForArguments<byte>(pArgsPayload, argsPayloadSize);

    /*
    for (gvirtus::common::mappedPointer mappedPointer:mappedPointers) {
        //printf("cudaLaunchKernel: marshall 0x%x size: %ld\n",mappedPointer.pointer,mappedPointer.size);
        CudaRtFrontend::AddHostPointerForArguments<byte>((byte *)mappedPointer.pointer, mappedPointer.size);
    }
     */

    //printf("Execute...\n");
    CudaRtFrontend::Execute("cudaLaunchKernel");
    cudaError = CudaRtFrontend::GetExitCode();
    //printf("...done!\n");
    if (cudaError == cudaSuccess) {
        printf("...cudaSuccess!\n");
        /*
        bool synchronized = false;
        for (int idx = 0; idx < infoFunction.params.size(); idx++) {
            if (CudaRtFrontend::isMappedMemory(*(void **) (args[idx]))) {

                void *hostPointer = *(void **) (args[idx]);

                //printf("cudaLaunchKernel: 0x%x is a mapped pointer!\n", hostPointer);

                gvirtus::common::mappedPointer mappedPointer = CudaRtFrontend::getMappedPointer(hostPointer);

                if (!synchronized) {
                    //printf("Synchronize\n");
                    cudaError = cudaDeviceSynchronize();
                    if (cudaError != cudaSuccess) {
                        cout << "cudaError:" << cudaError << endl;
                        break;
                    }
                    synchronized = true;
                }
                //printf("cudaLaunchKernel: cudaMemcpyAsync 0x%x (device) -> 0x%x (host) size:%ld\n",
                //       mappedPointer.pointer, hostPointer, mappedPointer.size);
                cudaError = cudaMemcpy(hostPointer, mappedPointer.pointer, mappedPointer.size,cudaMemcpyDeviceToHost);
                if (cudaError != cudaSuccess) {
                    break;
                }

            }
        }
         */
    }
    free(pArgsPayload);
    return cudaError;
}
#endif
