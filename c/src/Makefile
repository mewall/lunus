VPATH=${C_HOME}/src:${C_HOME}/lib:${C_HOME}/bin:${C_HOME}/include
CC=gcc-mp-4.8
#CC=gcc
CFLAGS= -g -fopenmp -DUSE_OPENMP -L${C_HOME}/lib -I${C_HOME}/include
#CFLAGS= -g -w -L${C_HOME}/lib -I${C_HOME}/include
AR=ar
ARFLAGS=-r
SHELL=/bin/csh
LIBS=-lmw -lm
DEPENDS=mwmask.h libmw.a
S=${C_HOME}/src
B=${C_HOME}/bin
L=${C_HOME}/lib
all: \
	abslt \
	addbeam \
	addosc \
	addtag \
	anisoult \
	avgim \
	avgpolim \
	avgrf \
	avgrim \
	avgrlt \
	avgsqim \
	avsqrim \
	avsqrlt \
	beamim \
	binasc \
	buttim \
	calcrmap \
	calcrsf \
	calcu \
	ccp4map2lat \
	ccrlt \
	constim \
	constlt \
	constrf \
	corrlt \
	cube2map \
	cullim \
	culllt \
	cutim \
	dat2vtk \
	decimap \
	df2im \
	dfrflt \
	dfsqrlt \
	diffim \
	divlt \
	divrf \
	explt \
	fftim \
	fftlt \
	fftwlt \
	filter \
	fobsmap \
	ftfltim \
	gausslt \
	gencmd \
	genlat \
	genmap \
	gmparse \
	harsf \
	hdrlt \
	headc \
	histim \
	histlt \
	hkl2vtk \
	intdfim \
	intimdf \
	intxdslt \
	lat2hkl \
	lat2map \
	lat2vtk \
	liquidcorrlt \
	liquidfaclt \
	map2lat \
	mapbound \
	maskimg \
	medim \
	minrim \
	minrlt \
	mirrorlt \
	modeim \
	muldwflt \
	mulim \
	mullt \
	mulrf \
	mulrfim \
	mulsclt \
	nign \
	noiseim \
	normim \
	normlt \
	ovldim \
	pattim \
	peakim \
	polarim \
	punchim \
	rad2s \
	ratioim \
	raw2img \
	resizelt \
	revbytelt \
	revyim \
	rf2char \
	rf2lt \
	rfaclt \
	rmpkim \
	rotlt \
	rsccmap \
	scaleim \
	scalelt \
	scalerf \
	see \
	seecat \
	seesh \
	settag \
	shiftlt \
	shiftsflt \
	shim4lt \
	shimlt \
	smthim \
	solidlt \
	subenvlt \
	subim \
	sublt \
	subrf \
	subrfim \
	subrflt \
	sumim \
	sumlt \
	sumrf \
	swapmw \
	symlt \
	tag_overloads \
	tagim \
	taglt \
	tagneg \
	tailc \
	thrshim \
	thrshlt \
	totiff8 \
	translt \
	transmap \
	tst3d \
	vasp2xtal \
	vtk2cube \
	vtk2lat \
	waveim \
	windim \
	writehdr \
	xflt \
	xfmask \
	xpl2mw \
	libmw.a
#
# Executables:
#
abslt:abslt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addbeam:addbeam.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addosc:addosc.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addtag:addtag.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
anisoult:anisoult.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgim:avgim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgpolim:avgpolim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgrf:avgrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgrim:avgrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgrlt:avgrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgsqim:avgsqim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avsqrim:avsqrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avsqrlt:avsqrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
beamim:beamim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
binasc:binasc.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
buttim:buttim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
calcrmap:calcrmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
calcrsf:calcrsf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
calcu:calcu.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ccp4map2lat:ccp4map2lat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ccrlt:ccrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
constim:constim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
constlt:constlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
constrf:constrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
corrlt:corrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cube2map:cube2map.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cullim:cullim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
culllt:culllt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cutim:cutim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
dat2vtk:dat2vtk.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
decimap:decimap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
df2im:df2im.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
dfrflt:dfrflt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
dfsqrlt:dfsqrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
diffim:diffim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
divlt:divlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
divrf:divrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
explt:explt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
fftim:fftim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
fftlt:fftlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
fftwlt:fftwlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
filter:filter.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
fobsmap:fobsmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ftfltim:ftfltim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
gausslt:gausslt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
gencmd:gencmd.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
genlat:genlat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
genmap:genmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
gmparse:gmparse.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
harsf:harsf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
hdrlt:hdrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
headc:headc.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
histim:histim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
histlt:histlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
hkl2vtk:hkl2vtk.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
intdfim:intdfim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
intimdf:intimdf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
intxdslt:intxdslt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
lat2hkl:lat2hkl.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
lat2map:lat2map.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
lat2vtk:lat2vtk.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
liquidcorrlt:liquidcorrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
liquidfaclt:liquidfaclt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
map2lat:map2lat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mapbound:mapbound.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
maskimg:maskimg.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
medim:medim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
minrim:minrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
minrlt:minrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mirrorlt:mirrorlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
modeim:modeim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
muldwflt:muldwflt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mulim:mulim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mullt:mullt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mulrf:mulrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mulrfim:mulrfim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mulsclt:mulsclt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
nign:nign.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
noiseim:noiseim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
normim:normim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
normlt:normlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ovldim:ovldim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
pattim:pattim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
peakim:peakim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
polarim:polarim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
punchim:punchim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rad2s:rad2s.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ratioim:ratioim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
raw2img:raw2img.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
resizelt:resizelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
revbytelt:revbytelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
revyim:revyim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rf2char:rf2char.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rf2lt:rf2lt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rfaclt:rfaclt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rmpkim:rmpkim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rotlt:rotlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rsccmap:rsccmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scaleim:scaleim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scalelt:scalelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scalerf:scalerf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
see:see.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
seecat:seecat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
seesh:seesh.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
settag:settag.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
shiftlt:shiftlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
shiftsflt:shiftsflt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
shim4lt:shim4lt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
shimlt:shimlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
smthim:smthim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
solidlt:solidlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subenvlt:subenvlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subim:subim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sublt:sublt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subrf:subrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subrfim:subrfim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subrflt:subrflt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumim:sumim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumlt:sumlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumrf:sumrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
swapmw:swapmw.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
symlt:symlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tag_overloads:tag_overloads.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tagim:tagim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
taglt:taglt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tagneg:tagneg.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tailc:tailc.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
thrshim:thrshim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
thrshlt:thrshlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
totiff8:totiff8.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
translt:translt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
transmap:transmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tst3d:tst3d.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
vasp2xtal:vasp2xtal.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
vtk2cube:vtk2cube.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
vtk2lat:vtk2lat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
waveim:waveim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
windim:windim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
writehdr:writehdr.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
xflt:xflt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
xfmask:xfmask.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
xpl2mw:xpl2mw.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
#
# Library functions:
#
labslt.o:labslt.c mwmask.h
lanisoult.o:lanisoult.c mwmask.h
lavgim.o:lavgim.c mwmask.h
lavgpolim.o:lavgpolim.c mwmask.h
lavgrf.o:lavgrf.c mwmask.h
lavgrim.o:lavgrim.c mwmask.h
lavgrlt.o:lavgrlt.c mwmask.h
lavgsqim.o:lavgsqim.c mwmask.h
lavsqrim.o:lavsqrim.c mwmask.h
lavsqrlt.o:lavsqrlt.c mwmask.h
lbeamim.o:lbeamim.c mwmask.h
lbuttim.o:lbuttim.c mwmask.h
lcalcrsf.o:lcalcrsf.c mwmask.h
lccrlt.o:lccrlt.c mwmask.h
lchbyte.o:lchbyte.c mwmask.h
lconstim.o:lconstim.c mwmask.h
lconstlt.o:lconstlt.c mwmask.h
lconstrf.o:lconstrf.c mwmask.h
lcorrlt.o:lcorrlt.c mwmask.h
lcpltmap.o:lcpltmap.c mwmask.h
lcpmaplt.o:lcpmaplt.c mwmask.h
lcullim.o:lcullim.c mwmask.h
lculllt.o:lculllt.c mwmask.h
lcutim.o:lcutim.c mwmask.h
ldecimap.o:ldecimap.c mwmask.h
ldf2im.o:ldf2im.c mwmask.h
ldfrflt.o:ldfrflt.c mwmask.h
ldfsqrlt.o:ldfsqrlt.c mwmask.h
ldiffim.o:ldiffim.c mwmask.h
ldivlt.o:ldivlt.c mwmask.h
ldivrf.o:ldivrf.c mwmask.h
lexplt.o:lexplt.c mwmask.h
lfft.o:lfft.c mwmask.h
lfreeim.o:lfreeim.c mwmask.h
lfreelt.o:lfreelt.c mwmask.h
lgausslt.o:lgausslt.c mwmask.h
lgensv.o:lgensv.c mwmask.h
lgetanls.o:lgetanls.c mwmask.h
lgetmat.o:lgetmat.c mwmask.h
lgetovld.o:lgetovld.c mwmask.h
lgetpks.o:lgetpks.c mwmask.h
lgettag.o:lgettag.c mwmask.h
linitim.o:linitim.c mwmask.h
linitlt.o:linitlt.c mwmask.h
linitmap.o:linitmap.c mwmask.h
linitxs.o:linitxs.c mwmask.h
lintdfim.o:lintdfim.c mwmask.h
lintxdslt.o:lintxdslt.c mwmask.h
lliquidcorrlt.o:lliquidcorrlt.c mwmask.h
lliquidfaclt.o:lliquidfaclt.c mwmask.h
lmattools.o:lmattools.c mwmask.h
lmedim.o:lmedim.c mwmask.h
lmin.o:lmin.c mwmask.h
lminrim.o:lminrim.c mwmask.h
lminrlt.o:lminrlt.c mwmask.h
lmirrorlt.o:lmirrorlt.c mwmask.h
lmodeim.o:lmodeim.c mwmask.h
lmuldwflt.o:lmuldwflt.c mwmask.h
lmulim.o:lmulim.c mwmask.h
lmullt.o:lmullt.c mwmask.h
lmulrf.o:lmulrf.c mwmask.h
lmulrfim.o:lmulrfim.c mwmask.h
lmulsclt.o:lmulsclt.c mwmask.h
lnign.o:lnign.c mwmask.h
lnoiseim.o:lnoiseim.c mwmask.h
lnormim.o:lnormim.c mwmask.h
lnormlt.o:lnormlt.c mwmask.h
lparsecell.o:lparsecell.c mwmask.h
lparsecelllt.o:lparsecelllt.c mwmask.h
lpeakim.o:lpeakim.c mwmask.h
lpolarim.o:lpolarim.c mwmask.h
lpunch.o:lpunch.c mwmask.h
lpunchim.o:lpunchim.c mwmask.h
lratioim.o:lratioim.c mwmask.h
lreadcube.o:lreadcube.c mwmask.h
lreaddf.o:lreaddf.c mwmask.h
lreadhkl.o:lreadhkl.c mwmask.h
lreadim.o:lreadim.c mwmask.h
lreadlt.o:lreadlt.c mwmask.h
lreadmap.o:lreadmap.c mwmask.h
lreadrf.o:lreadrf.c mwmask.h
lreadvtk.o:lreadvtk.c mwmask.h
lreadxs.o:lreadxs.c mwmask.h
lresizelt.o:lresizelt.c mwmask.h
lrevyim.o:lrevyim.c mwmask.h
lrf2lt.o:lrf2lt.c mwmask.h
lrfaclt.o:lrfaclt.c mwmask.h
lrmpkim.o:lrmpkim.c mwmask.h
lrotlt.o:lrotlt.c mwmask.h
lrsccmap.o:lrsccmap.c mwmask.h
lrsrlt.o:lrsrlt.c mwmask.h
lscaleim.o:lscaleim.c mwmask.h
lscalelt.o:lscalelt.c mwmask.h
lsettag.o:lsettag.c mwmask.h
lshiftlt.o:lshiftlt.c mwmask.h
lshiftsflt.o:lshiftsflt.c mwmask.h
lshim4lt.o:lshim4lt.c mwmask.h
lshimlt.o:lshimlt.c mwmask.h
lsmthim.o:lsmthim.c mwmask.h
lsolidlt.o:lsolidlt.c mwmask.h
lsubenvlt.o:lsubenvlt.c mwmask.h
lsubim.o:lsubim.c mwmask.h
lsublt.o:lsublt.c mwmask.h
lsubrf.o:lsubrf.c mwmask.h
lsubrfim.o:lsubrfim.c mwmask.h
lsubrflt.o:lsubrflt.c mwmask.h
lsumim.o:lsumim.c mwmask.h
lsumlt.o:lsumlt.c mwmask.h
lsumrf.o:lsumrf.c mwmask.h
lsymlib.o:lsymlib.c mwmask.h
lsymlt.o:lsymlt.c mwmask.h
ltagim.o:ltagim.c mwmask.h
ltaglt.o:ltaglt.c mwmask.h
lthrshim.o:lthrshim.c mwmask.h
lthrshlt.o:lthrshlt.c mwmask.h
ltordata.o:ltordata.c mwmask.h
ltranslt.o:ltranslt.c mwmask.h
ltransmap.o:ltransmap.c mwmask.h
lupdbd.o:lupdbd.c mwmask.h
lwaveim.o:lwaveim.c mwmask.h
lwindim.o:lwindim.c mwmask.h
lwritecube.o:lwritecube.c mwmask.h
lwritedf.o:lwritedf.c mwmask.h
lwritehkl.o:lwritehkl.c mwmask.h
lwriteim.o:lwriteim.c mwmask.h
lwritelt.o:lwritelt.c mwmask.h
lwritemap.o:lwritemap.c mwmask.h
lwriterf.o:lwriterf.c mwmask.h
lwritesh.o:lwritesh.c mwmask.h
lwritevtk.o:lwritevtk.c mwmask.h
lxavgr.o:lxavgr.c mwmask.h
lxavgrim.o:lxavgrim.c mwmask.h
lxflt.o:lxflt.c mwmask.h
lxfmask.o:lxfmask.c mwmask.h
.c.o:
	$(CC) -c -o ${L}/$*.o $(CFLAGS) ${L}/$*.c
#
# Libraries:
#
libmw.a: \
	labslt.o \
	lanisoult.o \
	lavgim.o \
	lavgpolim.o \
	lavgrf.o \
	lavgrim.o \
	lavgrlt.o \
	lavgsqim.o \
	lavsqrim.o \
	lavsqrlt.o \
	lbeamim.o \
	lbuttim.o \
	lcalcrsf.o \
	lccrlt.o \
	lchbyte.o \
	lconstim.o \
	lconstlt.o \
	lconstrf.o \
	lcorrlt.o \
	lcpltmap.o \
	lcpmaplt.o \
	lcullim.o \
	lculllt.o \
	lcutim.o \
	ldecimap.o \
	ldf2im.o \
	ldfrflt.o \
	ldfsqrlt.o \
	ldiffim.o \
	ldivlt.o \
	ldivrf.o \
	lexplt.o \
	lfft.o \
	lfreeim.o \
	lfreelt.o \
	lgausslt.o \
	lgensv.o \
	lgetanls.o \
	lgetmat.o \
	lgetovld.o \
	lgetpks.o \
	lgettag.o \
	linitim.o \
	linitlt.o \
	linitmap.o \
	linitxs.o \
	lintdfim.o \
	lintxdslt.o \
	lliquidcorrlt.o \
	lliquidfaclt.o \
	lmattools.o \
	lmedim.o \
	lmin.o \
	lminrim.o \
	lminrlt.o \
	lmirrorlt.o \
	lmodeim.o \
	lmuldwflt.o \
	lmulim.o \
	lmullt.o \
	lmulrf.o \
	lmulrfim.o \
	lmulsclt.o \
	lnign.o \
	lnoiseim.o \
	lnormim.o \
	lnormlt.o \
	lparsecell.o \
	lparsecelllt.o \
	lpeakim.o \
	lpolarim.o \
	lpunch.o \
	lpunchim.o \
	lratioim.o \
	lreadcube.o \
	lreaddf.o \
	lreadhkl.o \
	lreadim.o \
	lreadlt.o \
	lreadmap.o \
	lreadrf.o \
	lreadvtk.o \
	lreadxs.o \
	lresizelt.o \
	lrevyim.o \
	lrf2lt.o \
	lrfaclt.o \
	lrmpkim.o \
	lrotlt.o \
	lrsccmap.o \
	lrsrlt.o \
	lscaleim.o \
	lscalelt.o \
	lsettag.o \
	lshiftlt.o \
	lshiftsflt.o \
	lshim4lt.o \
	lshimlt.o \
	lsmthim.o \
	lsolidlt.o \
	lsubenvlt.o \
	lsubim.o \
	lsublt.o \
	lsubrf.o \
	lsubrfim.o \
	lsubrflt.o \
	lsumim.o \
	lsumlt.o \
	lsumrf.o \
	lsymlib.o \
	lsymlt.o \
	ltagim.o \
	ltaglt.o \
	lthrshim.o \
	lthrshlt.o \
	ltordata.o \
	ltranslt.o \
	ltransmap.o \
	lupdbd.o \
	lwaveim.o \
	lwindim.o \
	lwritecube.o \
	lwritedf.o \
	lwritehkl.o \
	lwriteim.o \
	lwritelt.o \
	lwritemap.o \
	lwriterf.o \
	lwritesh.o \
	lwritevtk.o \
	lxavgr.o \
	lxavgrim.o \
	lxflt.o \
	lxfmask.o 
	cd ${L}; rm libmw.a; $(AR) $(ARFLAGS) libmw.a *.o
