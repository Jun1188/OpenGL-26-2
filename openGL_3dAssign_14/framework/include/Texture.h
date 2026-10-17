#pragma once
#include "Model.h"
#include "../detail/Native.h"
#include <wincodec.h>
#include <stdexcept>
#include <algorithm>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace graphics {
namespace detail {
template<class T> struct ComOwner {
    T* value = nullptr;
    ~ComOwner() { if(value) value->Release(); }
    ComOwner() = default;
    ComOwner(const ComOwner&) = delete;
    ComOwner& operator=(const ComOwner&) = delete;
};
inline void checkCom(HRESULT result) {
    if(FAILED(result)) throw std::runtime_error("Image decoder failed (HRESULT "+std::to_string(result)+").");
}
struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    ComApartment() { if(result!=RPC_E_CHANGED_MODE) checkCom(result); }
    ~ComApartment() { if(SUCCEEDED(result)) CoUninitialize(); }
};
inline TextureImage decodeImage(const std::filesystem::path& file,
                               const std::vector<std::uint8_t>* encoded,
                               bool flipRows) {
    ComApartment apartment;
    ComOwner<IWICImagingFactory> factory;
    checkCom(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,
                             IID_PPV_ARGS(&factory.value)));
    ComOwner<IWICStream> stream;
    ComOwner<IWICBitmapDecoder> decoder;
    if(encoded) {
        if(encoded->empty() || encoded->size()>std::numeric_limits<DWORD>::max())
            throw std::runtime_error("Invalid encoded texture size.");
        checkCom(factory.value->CreateStream(&stream.value));
        checkCom(stream.value->InitializeFromMemory(const_cast<BYTE*>(encoded->data()),DWORD(encoded->size())));
        checkCom(factory.value->CreateDecoderFromStream(stream.value,nullptr,WICDecodeMetadataCacheOnLoad,&decoder.value));
    } else checkCom(factory.value->CreateDecoderFromFilename(file.c_str(),nullptr,GENERIC_READ,
                                                            WICDecodeMetadataCacheOnLoad,&decoder.value));
    ComOwner<IWICBitmapFrameDecode> frame;
    ComOwner<IWICFormatConverter> converter;
    checkCom(decoder.value->GetFrame(0,&frame.value));
    checkCom(factory.value->CreateFormatConverter(&converter.value));
    checkCom(converter.value->Initialize(frame.value,GUID_WICPixelFormat32bppRGBA,WICBitmapDitherTypeNone,
                                        nullptr,0,WICBitmapPaletteTypeCustom));
    TextureImage image;
    checkCom(converter.value->GetSize(&image.width,&image.height));
    if(!image.width || !image.height || image.width>16384 || image.height>16384)
        throw std::runtime_error("Texture dimensions must be within 1..16384.");
    image.rgba.resize(std::size_t(image.width)*image.height*4);
    checkCom(converter.value->CopyPixels(nullptr,image.width*4,UINT(image.rgba.size()),image.rgba.data()));
    if(flipRows) {
        const std::size_t stride=std::size_t(image.width)*4;
        for(std::uint32_t y=0;y<image.height/2;++y)
            std::swap_ranges(image.rgba.begin()+y*stride,image.rgba.begin()+(y+1)*stride,
                             image.rgba.begin()+(image.height-1-y)*stride);
    }
    return image;
}
} // namespace detail
} // namespace graphics
