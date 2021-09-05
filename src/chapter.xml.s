.section stamp
# This is the magic number 1LMG
smlaldxmi	r4, sp, r1, ip
andeq		r0, r0, r0

.section main
eorne		r0, r2, r5, lsl r0
eorne		r0, r8, r2, rrx
rsbeq		r0, r2, r0
andeq		r1, r0, fp, lsr #32
eorne		r0, sl, r2, rrx
rsbeq		r0, r2, r0
andeq		r1, r0, fp, lsr #32
eorne		r0, ip, r2, rrx
rsbeq		r0, r2, r0
andeq		r1, r0, sp, lsr #32
eorne		r0, lr, r2, rrx
rsbeq		r0, r2, r0
andeq		r1, r0, sp, lsr #32
eorne		r0, r0, r2, rrx
rsbeq		r0, r2, r0
