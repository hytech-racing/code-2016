#include <stddef.h> 
typedef unsigned char byte;
# define OP_T_THRES 16
# define op_t   unsigned long int
# define OPSIZ  (sizeof(op_t))
#define PAGE_COPY_THRESHOLD   0
#define PAGE_SIZE 0
# define PAGE_OFFSET(n) ((n) & (PAGE_SIZE - 1))
#define BYTE_COPY_FWD(dst_bp, src_bp, nbytes)                     \
      do                                          \
        {                                         \
          size_t __nbytes = (nbytes);                         \
          while (__nbytes > 0)                            \
        {                                     \
          byte __x = ((byte *) src_bp)[0];                    \
          src_bp += 1;                                \
          __nbytes -= 1;                              \
          ((byte *) dst_bp)[0] = __x;                         \
          dst_bp += 1;                                \
        }                                     \
        } while (0)
        
        #define BYTE_COPY_BWD(dst_ep, src_ep, nbytes)                     \
     do                                          \
       {                                         \
         size_t __nbytes = (nbytes);                         \
         while (__nbytes > 0)                            \
       {                                     \
         byte __x;                               \
         src_ep -= 1;                                \
         __x = ((byte *) src_ep)[0];                         \
         dst_ep -= 1;                                \
         __nbytes -= 1;                              \
         ((byte *) dst_ep)[0] = __x;                         \
       }                                     \
       } while (0)
        
        # define PAGE_COPY_FWD_MAYBE(dstp, srcp, nbytes_left, nbytes)             \
     do                                          \
       {                                         \
         if ((nbytes) >= PAGE_COPY_THRESHOLD &&                      \
        PAGE_OFFSET ((dstp) - (srcp)) == 0)                     \
       {                                     \
         /* The amount to copy is past the threshold for copying         \
            pages virtually with kernel VM operations, and the           \
            source and destination addresses have the same alignment.  */    \
         size_t nbytes_before = PAGE_OFFSET (-(dstp));               \
         if (nbytes_before != 0)                         \
           {                                     \
             /* First copy the words before the first page boundary.  */     \
             WORD_COPY_FWD (dstp, srcp, nbytes_left, nbytes_before);         \
             assert (nbytes_left == 0);                      \
             nbytes -= nbytes_before;                        \
           }                                     \
         PAGE_COPY_FWD (dstp, srcp, nbytes_left, nbytes);            \
       }                                     \
       } while (0)
        
        
        
        #define WORD_COPY_FWD(dst_bp, src_bp, nbytes_left, nbytes)            \
     do                                          \
       {                                         \
         if (src_bp % OPSIZ == 0)                            \
       _wordcopy_fwd_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);         \
         else                                    \
       _wordcopy_fwd_dest_aligned (dst_bp, src_bp, (nbytes) / OPSIZ);        \
         src_bp += (nbytes) & -OPSIZ;                        \
         dst_bp += (nbytes) & -OPSIZ;                        \
         (nbytes_left) = (nbytes) % OPSIZ;                       \
       } while (0)
        
        
        
        
        
        
  void memcpy (void *dstpp, const void *srcpp, size_t len) {
   unsigned long int dstp = (long int) dstpp;
   unsigned long int srcp = (long int) srcpp;
 
   /* Copy from the beginning to the end.  */
 
   /* If there not too few bytes to copy, use word copy.  */
   if (len >= OP_T_THRES)
     {
       /* Copy just a few bytes to make DSTP aligned.  */
       len -= (-dstp) % OPSIZ;
       BYTE_COPY_FWD (dstp, srcp, (-dstp) % OPSIZ);
 
       /* Copy whole pages from SRCP to DSTP by virtual address manipulation,
      as much as possible.  */
 
       PAGE_COPY_FWD_MAYBE (dstp, srcp, len, len);
 
       /* Copy from SRCP to DSTP taking advantage of the known alignment of
      DSTP.  Number of bytes remaining is put in the third argument,
      i.e. in LEN.  This number may vary from machine to machine.  */
 
       WORD_COPY_FWD (dstp, srcp, len, len);
 
       /* Fall out and copy the tail.  */
     }
 
   /* There are just a few bytes to copy.  Use byte memory operations.  */
   BYTE_COPY_FWD (dstp, srcp, len);
 
   return dstpp;
 }
