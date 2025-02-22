#pragma once

#include "dxvk_barrier.h"
#include "dxvk_bind_mask.h"
#include "dxvk_cmdlist.h"
#include "dxvk_context_state.h"
#include "dxvk_data.h"
#include "dxvk_objects.h"
#include "dxvk_util.h"

namespace dxvk {
  
  /**
   * \brief DXVk context
   * 
   * Tracks pipeline state and records command lists.
   * This is where the actual rendering commands are
   * recorded.
   */
  class DxvkContext : public RcObject {
    
  public:
    
    DxvkContext(const Rc<DxvkDevice>& device);
    ~DxvkContext();
    
    /**
     * \brief Begins command buffer recording
     * 
     * Begins recording a command list. This does
     * not alter any context state other than the
     * active command list.
     * \param [in] cmdList Target command list
     */
    void beginRecording(
      const Rc<DxvkCommandList>& cmdList);
    
    /**
     * \brief Ends command buffer recording
     * 
     * Finishes recording the active command list.
     * The command list can then be submitted to
     * the device.
     * 
     * This will not change any context state
     * other than the active command list.
     * \returns Active command list
     */
    Rc<DxvkCommandList> endRecording();

    /**
     * \brief Flushes command buffer
     * 
     * Transparently submits the current command
     * buffer and allocates a new one.
     */
    void flushCommandList();
    
    /**
     * \brief Begins generating query data
     * \param [in] query The query to end
     */
    void beginQuery(
      const Rc<DxvkGpuQuery>&   query);
    
    /**
     * \brief Ends generating query data
     * \param [in] query The query to end
     */
    void endQuery(
      const Rc<DxvkGpuQuery>&   query);
    
    /**
     * \brief Sets render targets
     * 
     * Creates a framebuffer on the fly if necessary
     * and binds it using \c bindFramebuffer. Set the
     * \c spill flag in order to make shader writes
     * from previous rendering operations visible.
     * \param [in] targets Render targets to bind
     * \param [in] spill Spill render pass if true
     */
    void bindRenderTargets(
      const DxvkRenderTargets&    targets,
            bool                  spill);
    
    /**
     * \brief Binds indirect argument buffer
     * 
     * Sets the buffers that are going to be used
     * for indirect draw and dispatch operations.
     * \param [in] argBuffer New argument buffer
     * \param [in] cntBuffer New count buffer
     */
    void bindDrawBuffers(
      const DxvkBufferSlice&      argBuffer,
      const DxvkBufferSlice&      cntBuffer);
    
    /**
     * \brief Binds index buffer
     * 
     * The index buffer will be used when
     * issuing \c drawIndexed commands.
     * \param [in] buffer New index buffer
     * \param [in] indexType Index type
     */
    void bindIndexBuffer(
      const DxvkBufferSlice&      buffer,
            VkIndexType           indexType);
    
    /**
     * \brief Binds buffer as a shader resource
     * 
     * Can be used for uniform and storage buffers.
     * \param [in] slot Resource binding slot
     * \param [in] buffer Buffer to bind
     */
    void bindResourceBuffer(
            uint32_t              slot,
      const DxvkBufferSlice&      buffer);
    
    /**
     * \brief Binds image or buffer view
     * 
     * Can be used for sampled images with a dedicated
     * sampler and for storage images, as well as for
     * uniform texel buffers and storage texel buffers.
     * \param [in] slot Resource binding slot
     * \param [in] imageView Image view to bind
     * \param [in] bufferView Buffer view to bind
     */
    void bindResourceView(
            uint32_t              slot,
      const Rc<DxvkImageView>&    imageView,
      const Rc<DxvkBufferView>&   bufferView);
    
    /**
     * \brief Binds image sampler
     * 
     * Binds a sampler that can be used together with
     * an image in order to read from a texture.
     * \param [in] slot Resource binding slot
     * \param [in] sampler Sampler view to bind
     */
    void bindResourceSampler(
            uint32_t              slot,
      const Rc<DxvkSampler>&      sampler);
    
    /**
     * \brief Binds a shader to a given state
     * 
     * \param [in] stage Target shader stage
     * \param [in] shader The shader to bind
     */
    void bindShader(
            VkShaderStageFlagBits stage,
      const Rc<DxvkShader>&       shader);
    
    /**
     * \brief Binds vertex buffer
     * 
     * \param [in] binding Vertex buffer binding
     * \param [in] buffer New vertex buffer
     * \param [in] stride Stride between vertices
     */
    void bindVertexBuffer(
            uint32_t              binding,
      const DxvkBufferSlice&      buffer,
            uint32_t              stride);
    
    /**
     * \brief Binds transform feedback buffer
     * 
     * \param [in] binding Xfb buffer binding
     * \param [in] buffer The buffer to bind
     * \param [in] counter Xfb counter buffer
     */
    void bindXfbBuffer(
            uint32_t              binding,
      const DxvkBufferSlice&      buffer,
      const DxvkBufferSlice&      counter);
    
    /**
     * \brief Blits an image
     * 
     * \param [in] dstImage Destination image
     * \param [in] srcImage Source image
     * \param [in] region Blit region
     * \param [in] filter Texture filter
     */
    void blitImage(
      const Rc<DxvkImage>&        dstImage,
      const Rc<DxvkImage>&        srcImage,
      const VkImageBlit&          region,
            VkFilter              filter);
    
    /**
     * \brief Changes image layout
     * 
     * Permanently changes the layout for a given
     * image. Immediately performs the transition.
     * \param [in] image The image to transition
     * \param [in] layout New image layout
     */
    void changeImageLayout(
      const Rc<DxvkImage>&        image,
            VkImageLayout         layout);
    
    /**
     * \brief Clears a buffer with a fixed value
     * 
     * Note that both \c offset and \c length must
     * be multiples of four, and that \c value is
     * consumed as a four-byte word.
     * \param [in] buffer The buffer to clear
     * \param [in] offset Offset of the range to clear
     * \param [in] length Bumber of bytes to clear
     * \param [in] value Clear value
     */
    void clearBuffer(
      const Rc<DxvkBuffer>&       buffer,
            VkDeviceSize          offset,
            VkDeviceSize          length,
            uint32_t              value);
    
    /**
     * \brief Clears a buffer view
     * 
     * Unlike \c clearBuffer, this method can be used
     * to clear a buffer view with format conversion. 
     * \param [in] bufferView The buffer view
     * \param [in] offset Offset of the region to clear
     * \param [in] length Extent of the region to clear
     * \param [in] value The clear value
     */
    void clearBufferView(
      const Rc<DxvkBufferView>&   bufferView,
            VkDeviceSize          offset,
            VkDeviceSize          length,
            VkClearColorValue     value);
    
    /**
     * \brief Clears subresources of a color image
     * 
     * \param [in] image The image to clear
     * \param [in] value Clear value
     * \param [in] subresources Subresources to clear
     */
    void clearColorImage(
      const Rc<DxvkImage>&            image,
      const VkClearColorValue&        value,
      const VkImageSubresourceRange&  subresources);
    
    /**
     * \brief Clears subresources of a depth-stencil image
     * 
     * \param [in] image The image to clear
     * \param [in] value Clear value
     * \param [in] subresources Subresources to clear
     */
    void clearDepthStencilImage(
      const Rc<DxvkImage>&            image,
      const VkClearDepthStencilValue& value,
      const VkImageSubresourceRange&  subresources);
    
    /**
     * \brief Clears a compressed image to black
     *
     * \param [in] image The image to clear
     * \param [in] subresources Subresources to clear
     */
    void clearCompressedColorImage(
      const Rc<DxvkImage>&            image,
      const VkImageSubresourceRange&  subresources);
    
    /**
     * \brief Clears an active render target
     * 
     * \param [in] imageView Render target view to clear
     * \param [in] clearAspects Image aspects to clear
     * \param [in] clearValue The clear value
     */
    void clearRenderTarget(
      const Rc<DxvkImageView>&    imageView,
            VkImageAspectFlags    clearAspects,
            VkClearValue          clearValue);
    
    /**
     * \brief Clears an image view
     * 
     * Can be used to clear sub-regions of storage images
     * that are not going to be used as render targets.
     * Implicit format conversion will be applied.
     * \param [in] imageView The image view
     * \param [in] offset Offset of the rect to clear
     * \param [in] extent Extent of the rect to clear
     * \param [in] aspect Aspect mask to clear
     * \param [in] value The clear value
     */
    void clearImageView(
      const Rc<DxvkImageView>&    imageView,
            VkOffset3D            offset,
            VkExtent3D            extent,
            VkImageAspectFlags    aspect,
            VkClearValue          value);
    
    /**
     * \brief Copies data from one buffer to another
     * 
     * \param [in] dstBuffer Destination buffer
     * \param [in] dstOffset Destination data offset
     * \param [in] srcBuffer Source buffer
     * \param [in] srcOffset Source data offset
     * \param [in] numBytes Number of bytes to copy
     */
    void copyBuffer(
      const Rc<DxvkBuffer>&       dstBuffer,
            VkDeviceSize          dstOffset,
      const Rc<DxvkBuffer>&       srcBuffer,
            VkDeviceSize          srcOffset,
            VkDeviceSize          numBytes);
    
    /**
     * \brief Copies overlapping buffer region
     * 
     * Can be used to copy potentially overlapping
     * buffer regions within the same buffer. If
     * the source and destination regions do not
     * overlap, it will behave as \ref copyBuffer.
     * \param [in] dstBuffer The buffer
     * \param [in] dstOffset Offset of target region
     * \param [in] srcOffset Offset of source region
     * \param [in] numBytes Number of bytes to copy
     */
    void copyBufferRegion(
      const Rc<DxvkBuffer>&       dstBuffer,
            VkDeviceSize          dstOffset,
            VkDeviceSize          srcOffset,
            VkDeviceSize          numBytes);
    
    /**
     * \brief Copies data from a buffer to an image
     * 
     * \param [in] dstImage Destination image
     * \param [in] dstSubresource Destination subresource
     * \param [in] dstOffset Destination area offset
     * \param [in] dstExtent Destination area size
     * \param [in] srcBuffer Source buffer
     * \param [in] srcOffset Source offset, in bytes
     * \param [in] srcExtent Source data extent
     */
    void copyBufferToImage(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset3D            dstOffset,
            VkExtent3D            dstExtent,
      const Rc<DxvkBuffer>&       srcBuffer,
            VkDeviceSize          srcOffset,
            VkExtent2D            srcExtent);
    
    /**
     * \brief Copies data from one image to another
     * 
     * \param [in] dstImage Destination image
     * \param [in] dstSubresource Destination subresource
     * \param [in] dstOffset Destination area offset
     * \param [in] srcImage Source image
     * \param [in] srcSubresource Source subresource
     * \param [in] srcOffset Source area offset
     * \param [in] extent Size of the area to copy
     */
    void copyImage(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset3D            dstOffset,
      const Rc<DxvkImage>&        srcImage,
            VkImageSubresourceLayers srcSubresource,
            VkOffset3D            srcOffset,
            VkExtent3D            extent);
    
    /**
     * \brief Copies overlapping image region
     *
     * \param [in] dstImage The image
     * \param [in] dstSubresource The image subresource
     * \param [in] dstOffset Destination region offset
     * \param [in] srcOffset Source region offset
     * \param [in] extent Size of the copy region
     */
    void copyImageRegion(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset3D            dstOffset,
            VkOffset3D            srcOffset,
            VkExtent3D            extent);
    
    /**
     * \brief Copies data from an image into a buffer
     * 
     * \param [in] dstBuffer Destination buffer
     * \param [in] dstOffset Destination offset, in bytes
     * \param [in] dstExtent Destination data extent
     * \param [in] srcImage Source image
     * \param [in] srcSubresource Source subresource
     * \param [in] srcOffset Source area offset
     * \param [in] srcExtent Source area size
     */
    void copyImageToBuffer(
      const Rc<DxvkBuffer>&       dstBuffer,
            VkDeviceSize          dstOffset,
            VkExtent2D            dstExtent,
      const Rc<DxvkImage>&        srcImage,
            VkImageSubresourceLayers srcSubresource,
            VkOffset3D            srcOffset,
            VkExtent3D            srcExtent);
    
    /**
     * \brief Packs depth-stencil image data to a buffer
     * 
     * Packs data from both the depth and stencil aspects
     * of an image into a buffer. The supported formats are:
     * - \c VK_FORMAT_D24_UNORM_S8_UINT: 0xssdddddd
     * - \c VK_FORMAT_D32_SFLOAT_S8_UINT: 0xdddddddd 0x000000ss
     * \param [in] dstBuffer Destination buffer
     * \param [in] dstOffset Destination offset, in bytes
     * \param [in] srcImage Source image
     * \param [in] srcSubresource Source subresource
     * \param [in] srcOffset Source area offset
     * \param [in] srcExtent Source area size
     * \param [in] format Packed data format
     */
    void copyDepthStencilImageToPackedBuffer(
      const Rc<DxvkBuffer>&       dstBuffer,
            VkDeviceSize          dstOffset,
      const Rc<DxvkImage>&        srcImage,
            VkImageSubresourceLayers srcSubresource,
            VkOffset2D            srcOffset,
            VkExtent2D            srcExtent,
            VkFormat              format);
    
    /**
     * \brief Unpacks buffer data to a depth-stencil image
     * 
     * Writes the packed depth-stencil data to an image.
     * See \ref copyDepthStencilImageToPackedBuffer for
     * which formats are supported and how they are packed.
     * \param [in] dstImage Destination image
     * \param [in] dstSubresource Destination subresource
     * \param [in] dstOffset Image area offset
     * \param [in] dstExtent Image area size
     * \param [in] srcBuffer Packed data buffer
     * \param [in] srcOffset Packed data offset
     * \param [in] format Packed data format
     */
    void copyPackedBufferToDepthStencilImage(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset2D            dstOffset,
            VkExtent2D            dstExtent,
      const Rc<DxvkBuffer>&       srcBuffer,
            VkDeviceSize          srcOffset,
            VkFormat              format);
    
    /**
     * \brief Discards a buffer
     * 
     * Renames the buffer in case it is currently
     * used by the GPU in order to avoid having to
     * insert barriers before future commands using
     * the buffer.
     * \param [in] buffer The buffer to discard
     */
    void discardBuffer(
      const Rc<DxvkBuffer>&       buffer);
    
    /**
     * \brief Discards image subresources
     * 
     * Discards the current contents of the image
     * and performs a fast layout transition. This
     * may improve performance in some cases.
     * \param [in] image The image to discard
     * \param [in] subresources Image subresources
     */
    void discardImage(
      const Rc<DxvkImage>&          image,
            VkImageSubresourceRange subresources);
    
    /**
     * \brief Starts compute jobs
     * 
     * \param [in] x Number of threads in X direction
     * \param [in] y Number of threads in Y direction
     * \param [in] z Number of threads in Z direction
     */
    void dispatch(
            uint32_t          x,
            uint32_t          y,
            uint32_t          z);
    
    /**
     * \brief Indirect dispatch call
     * 
     * Takes arguments from a buffer. The buffer must contain
     * a structure of the type \c VkDispatchIndirectCommand.
     * \param [in] offset Draw buffer offset
     */
    void dispatchIndirect(
            VkDeviceSize      offset);
    
    /**
     * \brief Draws primitive without using an index buffer
     * 
     * \param [in] vertexCount Number of vertices to draw
     * \param [in] instanceCount Number of instances to render
     * \param [in] firstVertex First vertex in vertex buffer
     * \param [in] firstInstance First instance ID
     */
    void draw(
            uint32_t          vertexCount,
            uint32_t          instanceCount,
            uint32_t          firstVertex,
            uint32_t          firstInstance);
    
    /**
     * \brief Indirect draw call
     * 
     * Takes arguments from a buffer. The structure stored
     * in the buffer must be of type \c VkDrawIndirectCommand.
     * \param [in] offset Draw buffer offset
     * \param [in] count Number of draws
     * \param [in] stride Stride between dispatch calls
     */
    void drawIndirect(
            VkDeviceSize      offset,
            uint32_t          count,
            uint32_t          stride);
    
    /**
     * \brief Indirect draw call
     * 
     * Takes arguments from a buffer. The structure stored
     * in the buffer must be of type \c VkDrawIndirectCommand.
     * \param [in] offset Draw buffer offset
     * \param [in] countOffset Draw count offset
     * \param [in] maxCount Maximum number of draws
     * \param [in] stride Stride between dispatch calls
     */
    void drawIndirectCount(
            VkDeviceSize      offset,
            VkDeviceSize      countOffset,
            uint32_t          maxCount,
            uint32_t          stride);
    
    /**
     * \brief Draws primitives using an index buffer
     * 
     * \param [in] indexCount Number of indices to draw
     * \param [in] instanceCount Number of instances to render
     * \param [in] firstIndex First index within the index buffer
     * \param [in] vertexOffset Vertex ID that corresponds to index 0
     * \param [in] firstInstance First instance ID
     */
    void drawIndexed(
            uint32_t indexCount,
            uint32_t instanceCount,
            uint32_t firstIndex,
            uint32_t vertexOffset,
            uint32_t firstInstance);
    
    /**
     * \brief Indirect indexed draw call
     * 
     * Takes arguments from a buffer. The structure type for
     * the draw buffer is \c VkDrawIndexedIndirectCommand.
     * \param [in] offset Draw buffer offset
     * \param [in] count Number of draws
     * \param [in] stride Stride between dispatch calls
     */
    void drawIndexedIndirect(
            VkDeviceSize      offset,
            uint32_t          count,
            uint32_t          stride);
    
    /**
     * \brief Indirect indexed draw call
     * 
     * Takes arguments from a buffer. The structure type for
     * the draw buffer is \c VkDrawIndexedIndirectCommand.
     * \param [in] offset Draw buffer offset
     * \param [in] countOffset Draw count offset
     * \param [in] maxCount Maximum number of draws
     * \param [in] stride Stride between dispatch calls
     */
    void drawIndexedIndirectCount(
            VkDeviceSize      offset,
            VkDeviceSize      countOffset,
            uint32_t          maxCount,
            uint32_t          stride);
    
    /**
     * \brief Transform feddback draw call

     * \param [in] counterBuffer Xfb counter buffer
     * \param [in] counterDivisor Vertex stride
     * \param [in] counterBias Counter bias
     */
    void drawIndirectXfb(
      const DxvkBufferSlice&  counterBuffer,
            uint32_t          counterDivisor,
            uint32_t          counterBias);
    
    /**
     * \brief Generates mip maps
     * 
     * Uses blitting to generate lower mip levels from
     * the top-most mip level passed to this method.
     * \param [in] imageView The image to generate mips for
     */
    void generateMipmaps(
      const Rc<DxvkImageView>&        imageView);
    
    /**
     * \brief Initializes or invalidates an image
     * 
     * Sets up the image layout for future operations
     * while discarding any previous contents.
     * \param [in] image The image to initialize
     * \param [in] subresources Image subresources
     * \param [in] initialLayout Initial image layout
     */
    void initImage(
      const Rc<DxvkImage>&            image,
      const VkImageSubresourceRange&  subresources,
            VkImageLayout             initialLayout);
    
    /**
     * \brief Invalidates a buffer's contents
     * 
     * Discards a buffer's contents by replacing the
     * backing resource. This allows the host to access
     * the buffer while the GPU is still accessing the
     * original backing resource.
     * 
     * \warning If the buffer is used by another context,
     * invalidating it will result in undefined behaviour.
     * \param [in] buffer The buffer to invalidate
     * \param [in] slice New buffer slice handle
     */
    void invalidateBuffer(
      const Rc<DxvkBuffer>&           buffer,
      const DxvkBufferSliceHandle&    slice);
    
    /**
     * \brief Updates push constants
     * 
     * Updates the given push constant range.
     * \param [in] offset Byte offset of data to update
     * \param [in] size Number of bytes to update
     * \param [in] data Pointer to raw data
     */
    void pushConstants(
            uint32_t                  offset,
            uint32_t                  size,
      const void*                     data);
    
    /**
     * \brief Resolves a multisampled image resource
     * 
     * Resolves a multisampled image into a non-multisampled
     * image. The subresources of both images must have the
     * same size and compatible formats.
     * A format can be specified for the resolve operation.
     * If it is \c VK_FORMAT_UNDEFINED, the resolve operation
     * will use the source image format.
     * \param [in] dstImage Destination image
     * \param [in] srcImage Source image
     * \param [in] region Region to resolve
     * \param [in] format Format for the resolve operation
     */
    void resolveImage(
      const Rc<DxvkImage>&            dstImage,
      const Rc<DxvkImage>&            srcImage,
      const VkImageResolve&           region,
            VkFormat                  format);
    
    /**
     * \brief Resolves a multisampled depth-stencil resource
     * 
     * \param [in] dstImage Destination image
     * \param [in] srcImage Source image
     * \param [in] region Region to resolve
     * \param [in] depthMode Resolve mode for depth aspect
     * \param [in] stencilMode Resolve mode for stencil aspect
     */
    void resolveDepthStencilImage(
      const Rc<DxvkImage>&            dstImage,
      const Rc<DxvkImage>&            srcImage,
      const VkImageResolve&           region,
            VkResolveModeFlagBitsKHR  depthMode,
            VkResolveModeFlagBitsKHR  stencilMode);

    /**
     * \brief Transforms image subresource layouts
     * 
     * \param [in] dstImage Image to transform
     * \param [in] dstSubresources Subresources
     * \param [in] srcLayout Current layout
     * \param [in] dstLayout Desired layout
     */
    void transformImage(
      const Rc<DxvkImage>&            dstImage,
      const VkImageSubresourceRange&  dstSubresources,
            VkImageLayout             srcLayout,
            VkImageLayout             dstLayout);
    
    /**
     * \brief Updates a buffer
     * 
     * Copies data from the host into a buffer.
     * \param [in] buffer Destination buffer
     * \param [in] offset Offset of sub range to update
     * \param [in] size Length of sub range to update
     * \param [in] data Data to upload
     */
    void updateBuffer(
      const Rc<DxvkBuffer>&           buffer,
            VkDeviceSize              offset,
            VkDeviceSize              size,
      const void*                     data);
    
    /**
     * \brief Updates an image
     * 
     * Copies data from the host into an image.
     * \param [in] image Destination image
     * \param [in] subsresources Image subresources to update
     * \param [in] imageOffset Offset of the image area to update
     * \param [in] imageExtent Size of the image area to update
     * \param [in] data Source data
     * \param [in] pitchPerRow Row pitch of the source data
     * \param [in] pitchPerLayer Layer pitch of the source data
     */
    void updateImage(
      const Rc<DxvkImage>&            image,
      const VkImageSubresourceLayers& subresources,
            VkOffset3D                imageOffset,
            VkExtent3D                imageExtent,
      const void*                     data,
            VkDeviceSize              pitchPerRow,
            VkDeviceSize              pitchPerLayer);
    
    /**
     * \brief Updates an depth-stencil image
     * 
     * \param [in] image Destination image
     * \param [in] subsresources Image subresources to update
     * \param [in] imageOffset Offset of the image area to update
     * \param [in] imageExtent Size of the image area to update
     * \param [in] data Source data
     * \param [in] pitchPerRow Row pitch of the source data
     * \param [in] pitchPerLayer Layer pitch of the source data
     * \param [in] format Packed depth-stencil format
     */
    void updateDepthStencilImage(
      const Rc<DxvkImage>&            image,
      const VkImageSubresourceLayers& subresources,
            VkOffset2D                imageOffset,
            VkExtent2D                imageExtent,
      const void*                     data,
            VkDeviceSize              pitchPerRow,
            VkDeviceSize              pitchPerLayer,
            VkFormat                  format);
    
    /**
     * \brief Uses transfer queue to initialize buffer
     * 
     * Only safe to use if the buffer is not in use by the GPU.
     * \param [in] buffer The buffer to initialize
     * \param [in] data The data to copy to the buffer
     */
    void uploadBuffer(
      const Rc<DxvkBuffer>&           buffer,
      const void*                     data);
    
    /**
     * \brief Uses transfer queue to initialize image
     * 
     * Only safe to use if the image is not in use by the GPU.
     * \param [in] image The image to initialize
     * \param [in] subresources Subresources to initialize
     * \param [in] data Source data
     * \param [in] pitchPerRow Row pitch of the source data
     * \param [in] pitchPerLayer Layer pitch of the source data
     */
    void uploadImage(
      const Rc<DxvkImage>&            image,
      const VkImageSubresourceLayers& subresources,
      const void*                     data,
            VkDeviceSize              pitchPerRow,
            VkDeviceSize              pitchPerLayer);
    
    /**
     * \brief Sets viewports
     * 
     * \param [in] viewportCount Number of viewports
     * \param [in] viewports The viewports
     * \param [in] scissorRects Schissor rectangles
     */
    void setViewports(
            uint32_t            viewportCount,
      const VkViewport*         viewports,
      const VkRect2D*           scissorRects);
    
    /**
     * \brief Sets blend constants
     * 
     * Blend constants are a set of four floating
     * point numbers that may be used as an input
     * for blending operations.
     * \param [in] blendConstants Blend constants
     */
    void setBlendConstants(
            DxvkBlendConstants  blendConstants);
    
    /**
     * \brief Sets depth bias
     * 
     * Depth bias has to be enabled explicitly in
     * the rasterizer state to have any effect.
     * \param [in] depthBias Depth bias values
     */
    void setDepthBias(
            DxvkDepthBias       depthBias);
    
    /**
     * \brief Sets depth bounds
     *
     * Enables or disables the depth bounds test,
     * and updates the values if necessary.
     * \param [in] depthBounds Depth bounds
     */
    void setDepthBounds(
            DxvkDepthBounds     depthBounds);
    
    /**
     * \brief Sets stencil reference
     * 
     * Sets the reference value for stencil compare operations.
     * \param [in] reference Reference value
     */
    void setStencilReference(
            uint32_t            reference);
    
    /**
     * \brief Sets input assembly state
     * \param [in] ia New state object
     */
    void setInputAssemblyState(
      const DxvkInputAssemblyState& ia);
    
    /**
     * \brief Sets input layout
     * 
     * \param [in] attributeCount Number of vertex attributes
     * \param [in] attributes The vertex attributes
     * \param [in] bindingCount Number of buffer bindings
     * \param [in] bindings Vertex buffer bindigs
     */
    void setInputLayout(
            uint32_t             attributeCount,
      const DxvkVertexAttribute* attributes,
            uint32_t             bindingCount,
      const DxvkVertexBinding*   bindings);
    
    /**
     * \brief Sets rasterizer state
     * \param [in] rs New state object
     */
    void setRasterizerState(
      const DxvkRasterizerState& rs);
    
    /**
     * \brief Sets multisample state
     * \param [in] ms New state object
     */
    void setMultisampleState(
      const DxvkMultisampleState& ms);
    
    /**
     * \brief Sets depth stencil state
     * \param [in] ds New state object
     */
    void setDepthStencilState(
      const DxvkDepthStencilState& ds);
    
    /**
     * \brief Sets logic op state
     * \param [in] lo New state object
     */
    void setLogicOpState(
      const DxvkLogicOpState&   lo);
    
    /**
     * \brief Sets blend mode for an attachment
     * 
     * \param [in] attachment The attachment index
     * \param [in] blendMode The blend mode
     */
    void setBlendMode(
            uint32_t            attachment,
      const DxvkBlendMode&      blendMode);
    
    /**
     * \brief Sets specialization constants
     * 
     * Replaces current specialization constants with
     * the given list of constant entries. The specId
     * in the shader can be computed with \c getSpecId.
     * \param [in] pipeline Graphics or Compute pipeline
     * \param [in] index Constant index
     * \param [in] value Constant value
     */
    void setSpecConstant(
            VkPipelineBindPoint pipeline,
            uint32_t            index,
            uint32_t            value);
    
    /**
     * \brief Sets predicate
     *
     * Enables or disables conditional rendering,
     * depending on whether the given buffer slice
     * is defined or not. Draw calls and render
     * target clear commands will get discarded if
     * the predicate value is either zero or non-zero.
     * \param [in] predicate The predicate buffer
     * \param [in] flags Conditional rendering mode
     */
    void setPredicate(
      const DxvkBufferSlice&    predicate,
            VkConditionalRenderingFlagsEXT flags);
    
    /**
     * \brief Sets barrier control flags
     *
     * Barrier control flags can be used to control
     * implicit synchronization of compute shaders.
     * \param [in] control New barrier control flags
     */
    void setBarrierControl(
            DxvkBarrierControlFlags control);
    
    /**
     * \brief Signals a GPU event
     * \param [in] event The event
     */
    void signalGpuEvent(
      const Rc<DxvkGpuEvent>&   event);
    
    /**
     * \brief Copies query data to predicate buffer
     * 
     * The given buffer slice can then be passed
     * to \c setPredicate to enable predication.
     * \param [in] predicate Predicate buffer
     * \param [in] query Source query
     */
    void writePredicate(
      const DxvkBufferSlice&    predicate,
      const Rc<DxvkGpuQuery>&   query);
    
    /**
     * \brief Writes to a timestamp query
     * \param [in] query The timestamp query
     */
    void writeTimestamp(
      const Rc<DxvkGpuQuery>&   query);
    
    /**
     * \brief Queues a signal
     * 
     * The signal will be notified after all
     * previously submitted commands have
     * finished execution on the GPU.
     * \param [in] signal The signal
     */
    void queueSignal(
      const Rc<sync::Signal>&   signal);
    
    /**
     * \brief Trims staging buffers
     * 
     * Releases staging buffer resources. Calling
     * this may be useful if data updates on a
     * given context are rare.
     */
    void trimStagingBuffers();
    
  private:
    
    Rc<DxvkDevice>          m_device;
    DxvkObjects*            m_common;
    
    Rc<DxvkCommandList>     m_cmd;
    Rc<DxvkDescriptorPool>  m_descPool;

    DxvkContextFlags        m_flags;
    DxvkContextState        m_state;

    DxvkBarrierSet          m_sdmaAcquires;
    DxvkBarrierSet          m_sdmaBarriers;
    DxvkBarrierSet          m_initBarriers;
    DxvkBarrierSet          m_execAcquires;
    DxvkBarrierSet          m_execBarriers;
    DxvkBarrierControlFlags m_barrierControl;
    
    DxvkGpuQueryManager     m_queryManager;
    DxvkStagingDataAlloc    m_staging;
    
    VkPipeline m_gpActivePipeline = VK_NULL_HANDLE;
    VkPipeline m_cpActivePipeline = VK_NULL_HANDLE;

    VkDescriptorSet m_gpSet = VK_NULL_HANDLE;
    VkDescriptorSet m_cpSet = VK_NULL_HANDLE;

    DxvkBindingSet<MaxNumVertexBindings + 1>  m_vbTracked;
    DxvkBindingSet<MaxNumResourceSlots>       m_rcTracked;

    std::array<DxvkShaderResourceSlot, MaxNumResourceSlots>  m_rc;
    std::array<DxvkDescriptorInfo,     MaxNumActiveBindings> m_descInfos;
    std::array<uint32_t,               MaxNumActiveBindings> m_descOffsets;
    
    std::unordered_map<
      DxvkBufferSliceHandle,
      DxvkGpuQueryHandle,
      DxvkHash, DxvkEq>     m_predicateWrites;
    
    void clearImageViewFb(
      const Rc<DxvkImageView>&    imageView,
            VkOffset3D            offset,
            VkExtent3D            extent,
            VkImageAspectFlags    aspect,
            VkClearValue          value);
    
    void clearImageViewCs(
      const Rc<DxvkImageView>&    imageView,
            VkOffset3D            offset,
            VkExtent3D            extent,
            VkClearValue          value);
    
    void copyImageHw(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset3D            dstOffset,
      const Rc<DxvkImage>&        srcImage,
            VkImageSubresourceLayers srcSubresource,
            VkOffset3D            srcOffset,
            VkExtent3D            extent);
    
    void copyImageFb(
      const Rc<DxvkImage>&        dstImage,
            VkImageSubresourceLayers dstSubresource,
            VkOffset3D            dstOffset,
      const Rc<DxvkImage>&        srcImage,
            VkImageSubresourceLayers srcSubresource,
            VkOffset3D            srcOffset,
            VkExtent3D            extent);
    
    void resolveImageHw(
      const Rc<DxvkImage>&            dstImage,
      const Rc<DxvkImage>&            srcImage,
      const VkImageResolve&           region);
    
    void resolveImageDs(
      const Rc<DxvkImage>&            dstImage,
      const Rc<DxvkImage>&            srcImage,
      const VkImageResolve&           region,
            VkResolveModeFlagBitsKHR  depthMode,
            VkResolveModeFlagBitsKHR  stencilMode);
    
    void resolveImageFb(
      const Rc<DxvkImage>&            dstImage,
      const Rc<DxvkImage>&            srcImage,
      const VkImageResolve&           region,
            VkFormat                  format,
            VkResolveModeFlagBitsKHR  depthMode,
            VkResolveModeFlagBitsKHR  stencilMode);
    
    void updatePredicate(
      const DxvkBufferSliceHandle&    predicate,
      const DxvkGpuQueryHandle&       query);
    
    void commitPredicateUpdates();
    
    void startRenderPass();
    void spillRenderPass();
    void clearRenderPass();
    
    void renderPassBindFramebuffer(
      const Rc<DxvkFramebuffer>&  framebuffer,
      const DxvkRenderPassOps&    ops,
            uint32_t              clearValueCount,
      const VkClearValue*         clearValues);
    
    void renderPassUnbindFramebuffer();
    
    void resetRenderPassOps(
      const DxvkRenderTargets&    renderTargets,
            DxvkRenderPassOps&    renderPassOps);
    
    void startConditionalRendering();
    void pauseConditionalRendering();
    
    void startTransformFeedback();
    void pauseTransformFeedback();
    
    void unbindComputePipeline();
    void updateComputePipeline();
    void updateComputePipelineState();
    
    void unbindGraphicsPipeline();
    void updateGraphicsPipeline();
    void updateGraphicsPipelineState();
    
    void updateComputeShaderResources();
    void updateComputeShaderDescriptors();
    
    void updateGraphicsShaderResources();
    void updateGraphicsShaderDescriptors();

    template<VkPipelineBindPoint BindPoint>
    bool updateShaderResources(
      const DxvkPipelineLayout*     layout);
    
    VkDescriptorSet updateShaderDescriptors(
      const DxvkPipelineLayout*     layout);
    
    template<VkPipelineBindPoint BindPoint>
    void updateShaderDescriptorSetBinding(
            VkDescriptorSet         set,
      const DxvkPipelineLayout*     layout);

    void updateFramebuffer();
    
    void updateIndexBufferBinding();
    void updateVertexBufferBindings();

    void updateTransformFeedbackBuffers();
    void updateTransformFeedbackState();

    void updateConditionalRendering();
    
    void updateDynamicState();

    template<VkPipelineBindPoint BindPoint>
    void updatePushConstants();
    
    void commitComputeState();
    
    template<bool Indexed>
    void commitGraphicsState();
    
    void commitComputeInitBarriers();
    void commitComputePostBarriers();
    
    void commitGraphicsPostBarriers();

    void emitMemoryBarrier(
            VkPipelineStageFlags      srcStages,
            VkAccessFlags             srcAccess,
            VkPipelineStageFlags      dstStages,
            VkAccessFlags             dstAccess);
    
    VkDescriptorSet allocateDescriptorSet(
            VkDescriptorSetLayout     layout);

    void trackDrawBuffer();
    
  };
  
}