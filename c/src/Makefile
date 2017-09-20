VPATH=${C_HOME}/src:${C_HOME}/lib:${C_HOME}/bin:${C_HOME}/include
#CC=gcc-mp-4.8
#CC=gcc-mp-5
CC=gcc
#CC=cc
FFTWROOT=/Users/mewall/packages/fftw-3.3.4/inst_dir
FFTW_DIR=${FFTWROOT}/lib
FFTW_INC=${FFTWROOT}/include
#CFLAGS= -g -fopenmp -DUSE_FFTW -DUSE_OPENMP -L${FFTW_DIR} -I${FFTW_INC} -L${C_HOME}/lib -I${C_HOME}/include
#CFLAGS= -g -DUSE_FFTW -L${FFTW_DIR} -I${FFTW_INC} -L${C_HOME}/lib -I${C_HOME}/include
#CFLAGS= -g -O2 -axcore-avx2,mic-avx512 -DUSE_OPENMP -L${C_HOME}/lib -I${C_HOME}/include
CFLAGS= -g -O2 -DUSE_OPENMP -L${C_HOME}/lib -I${C_HOME}/include
#CFLAGS= -g -O2 -axcore-avx2,mic-avx512 -L${C_HOME}/lib -I${C_HOME}/include
#CFLAGS= -g -w -L${C_HOME}/lib -I${C_HOME}/include
AR=ar
ARFLAGS=-r
SHELL=/bin/csh
LIBS=-lmw -lm -lgomp
DEPENDS=mwmask.h libmw.a
S=${C_HOME}/src
B=${C_HOME}/bin
L=${C_HOME}/lib
all: \
	absim \
	abslt \
	addbeam \
	addosc \
	addtag \
	anisolt \
	anisoult \
	avgim \
	avgpolim \
	avgrf \
	avgrim_aniso \
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
	ccrim \
	ccrim_full \
	ccrlt \
	cfim \
	constim \
	constlt \
	constrf \
	corrlt \
	cube2map \
	cullconelt \
	cullim \
	culllt \
	cullreslt \
	cutim \
	dat2vtk \
	debraggim \
	detectormask \
	dfrflt \
	dfsqrlt \
	diffim \
	divlt \
	divrf \
	explt \
	fftim \
	fftlt \
	fftwlt \
	fillim \
	fillpixim \
	filter \
	fobsmap \
	ftfltim \
	gausslt \
	gencmd \
	genmap \
	getrim \
	harsf \
	hdrlt \
	headc \
	histim \
	histlt \
	hkl2lat \
	hkl2vtk \
	intxdslt \
	lat2hkl \
	lat2map \
	lat2vtk \
	liquidcorrlt \
	liquidfaclt \
	lowrim \
	lunus \
	makelt \
	map2lat \
	mapbound \
	maskimg \
	mathim \
	medim \
	minim \
	minrim \
	minrlt \
	mirrorlt \
	modeim \
	mulcfim \
	muldwflt \
	mulim \
	mullt \
	mulrf \
	mulrfim \
	mulscim \
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
	rbminrim \
	resizelt \
	revbytelt \
	revyim \
	rf2char2 \
	rf2char \
	rf2lt \
	rfaclt \
	rmpixim \
	rmpkim \
	rmsdim \
	rotlt \
	rsccmap \
	samplt \
	scaleim \
	scalelt \
	scalerf \
	scalerfim \
	see \
	seecat \
	seesh \
	setped \
	settag \
	shiftlt \
	shiftsflt \
	shim4lt \
	shimlt \
	smoothim \
	smthim \
	solidlt \
	splintrf \
	sqrtrf \
	subenvlt \
	subim \
	sublt \
	subminlt \
	subrf \
	subrfim \
	subrflt \
	sumim \
	sumlt \
	sumresim \
	sumrf \
	sumrim \
	sumscim \
	swapmw \
	symlt \
	symminlt \
	tagim \
	taglt \
	tagneg \
	tag_overloads \
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
	libmw.a
#
# Executables:
#
absim:absim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
abslt:abslt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addbeam:addbeam.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addosc:addosc.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
addtag:addtag.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
anisolt:anisolt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
anisoult:anisoult.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgim:avgim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgpolim:avgpolim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgrf:avgrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
avgrim_aniso:avgrim_aniso.c ${DEPENDS}
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
ccrim:ccrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ccrim_full:ccrim_full.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
ccrlt:ccrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cfim:cfim.c ${DEPENDS}
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
cullconelt:cullconelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cullim:cullim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
culllt:culllt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cullreslt:cullreslt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
cutim:cutim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
dat2vtk:dat2vtk.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
debraggim:debraggim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
detectormask:detectormask.c ${DEPENDS}
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
fillim:fillim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
fillpixim:fillpixim.c ${DEPENDS}
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
genmap:genmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
getrim:getrim.c ${DEPENDS}
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
hkl2lat:hkl2lat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
hkl2vtk:hkl2vtk.c ${DEPENDS}
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
lowrim:lowrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
lunus:lunus.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
makelt:makelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
map2lat:map2lat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mapbound:mapbound.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
maskimg:maskimg.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mathim:mathim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
medim:medim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
minim:minim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
minrim:minrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
minrlt:minrlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mirrorlt:mirrorlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
modeim:modeim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
mulcfim:mulcfim.c ${DEPENDS}
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
mulscim:mulscim.c ${DEPENDS}
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
rbminrim:rbminrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
resizelt:resizelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
revbytelt:revbytelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
revyim:revyim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rf2char2:rf2char2.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rf2char:rf2char.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rf2lt:rf2lt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rfaclt:rfaclt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rmpixim:rmpixim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rmpkim:rmpkim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rmsdim:rmsdim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rotlt:rotlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
rsccmap:rsccmap.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
samplt:samplt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scaleim:scaleim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scalelt:scalelt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scalerf:scalerf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
scalerfim:scalerfim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
see:see.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
seecat:seecat.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
seesh:seesh.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
setped:setped.c ${DEPENDS}
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
smoothim:smoothim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
smthim:smthim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
solidlt:solidlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
splintrf:splintrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sqrtrf:sqrtrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subenvlt:subenvlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subim:subim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sublt:sublt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
subminlt:subminlt.c ${DEPENDS}
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
sumresim:sumresim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumrf:sumrf.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumrim:sumrim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
sumscim:sumscim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
swapmw:swapmw.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
symlt:symlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
symminlt:symminlt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tagim:tagim.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
taglt:taglt.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tagneg:tagneg.c ${DEPENDS}
	${CC} ${CFLAGS} -o ${B}/$@ ${S}/$@.c ${LIBS}
tag_overloads:tag_overloads.c ${DEPENDS}
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
#
# Library functions:
#
labsim.o:labsim.c lunus.h
labslt.o:labslt.c lunus.h
lanisolt.o:lanisolt.c lunus.h
lanisoult.o:lanisoult.c lunus.h
lavgim.o:lavgim.c lunus.h
lavgpolim.o:lavgpolim.c lunus.h
lavgrf.o:lavgrf.c lunus.h
lavgrim_aniso.o:lavgrim_aniso.c lunus.h
lavgrim.o:lavgrim.c lunus.h
lavgrlt.o:lavgrlt.c lunus.h
lavgsqim.o:lavgsqim.c lunus.h
lavsqrim.o:lavsqrim.c lunus.h
lavsqrlt.o:lavsqrlt.c lunus.h
lbeamim.o:lbeamim.c lunus.h
lbuttim.o:lbuttim.c lunus.h
lcalcrsf.o:lcalcrsf.c lunus.h
lcbftool.o:lcbftool.c lunus.h
lccrim.o:lccrim.c lunus.h
lccrim_helper.o:lccrim_helper.c lunus.h
lccrlt.o:lccrlt.c lunus.h
lcfim.o:lcfim.c lunus.h
lchbyte.o:lchbyte.c lunus.h
lconstim.o:lconstim.c lunus.h
lconstlt.o:lconstlt.c lunus.h
lconstrf.o:lconstrf.c lunus.h
lcorrlt.o:lcorrlt.c lunus.h
lcpltmap.o:lcpltmap.c lunus.h
lcpmaplt.o:lcpmaplt.c lunus.h
lcullconelt.o:lcullconelt.c lunus.h
lcullim.o:lcullim.c lunus.h
lculllt.o:lculllt.c lunus.h
lcullreslt.o:lcullreslt.c lunus.h
lcutim.o:lcutim.c lunus.h
ldebraggim.o:ldebraggim.c lunus.h
ldfrflt.o:ldfrflt.c lunus.h
ldfsqrlt.o:ldfsqrlt.c lunus.h
ldiffim.o:ldiffim.c lunus.h
ldivlt.o:ldivlt.c lunus.h
ldivrf.o:ldivrf.c lunus.h
lexplt.o:lexplt.c lunus.h
lfft.o:lfft.c lunus.h
lfillim.o:lfillim.c lunus.h
lfillpixim.o:lfillpixim.c lunus.h
lfreeim.o:lfreeim.c lunus.h
lfreelt.o:lfreelt.c lunus.h
lfrmsdim.o:lfrmsdim.c lunus.h
lgausslt.o:lgausslt.c lunus.h
lgensv.o:lgensv.c lunus.h
lgetanls.o:lgetanls.c lunus.h
lgetmat.o:lgetmat.c lunus.h
lgetovld.o:lgetovld.c lunus.h
lgetpix.o:lgetpix.c lunus.h
lgetpks.o:lgetpks.c lunus.h
lgetrim.o:lgetrim.c lunus.h
lgettag.o:lgettag.c lunus.h
linitim.o:linitim.c lunus.h
linitlt.o:linitlt.c lunus.h
linitmap.o:linitmap.c lunus.h
linitxs.o:linitxs.c lunus.h
lintxdslt.o:lintxdslt.c lunus.h
lliquidcorrlt.o:lliquidcorrlt.c lunus.h
lliquidfaclt.o:lliquidfaclt.c lunus.h
llowrim.o:llowrim.c lunus.h
lmathim.o:lmathim.c lunus.h
lmattools.o:lmattools.c lunus.h
lmedim.o:lmedim.c lunus.h
lmin.o:lmin.c lunus.h
lminim.o:lminim.c lunus.h
lminrim.o:lminrim.c lunus.h
lminrlt.o:lminrlt.c lunus.h
lmirrorlt.o:lmirrorlt.c lunus.h
lmodeim.o:lmodeim.c lunus.h
lmulcfim.o:lmulcfim.c lunus.h
lmuldwflt.o:lmuldwflt.c lunus.h
lmulim.o:lmulim.c lunus.h
lmullt.o:lmullt.c lunus.h
lmulrf.o:lmulrf.c lunus.h
lmulrfim.o:lmulrfim.c lunus.h
lmulscim.o:lmulscim.c lunus.h
lmulsclt.o:lmulsclt.c lunus.h
lnign.o:lnign.c lunus.h
lnoiseim.o:lnoiseim.c lunus.h
lnormim.o:lnormim.c lunus.h
lnormlt.o:lnormlt.c lunus.h
lp6mmask.o:lp6mmask.c lunus.h
lparallel.o:lparallel.c lunus.h
lparsecelllt.o:lparsecelllt.c lunus.h
lpeakim.o:lpeakim.c lunus.h
lpolarim.o:lpolarim.c lunus.h
lpunch.o:lpunch.c lunus.h
lpunchim.o:lpunchim.c lunus.h
lratioim.o:lratioim.c lunus.h
lrbminrim.o:lrbminrim.c lunus.h
lreadcbf.o:lreadcbf.c lunus.h
lreadcube.o:lreadcube.c lunus.h
lreadhkl.o:lreadhkl.c lunus.h
lreadim.o:lreadim.c lunus.h
lreadim_orig.o:lreadim_orig.c lunus.h
lreadlt.o:lreadlt.c lunus.h
lreadmap.o:lreadmap.c lunus.h
lreadrf.o:lreadrf.c lunus.h
lreadvtk.o:lreadvtk.c lunus.h
lreadxs.o:lreadxs.c lunus.h
lresizelt.o:lresizelt.c lunus.h
lrevyim.o:lrevyim.c lunus.h
lrf2lt.o:lrf2lt.c lunus.h
lrfaclt.o:lrfaclt.c lunus.h
lrmpkim.o:lrmpkim.c lunus.h
lrmsdim.o:lrmsdim.c lunus.h
lrotlt.o:lrotlt.c lunus.h
lrsccmap.o:lrsccmap.c lunus.h
lrsrlt.o:lrsrlt.c lunus.h
lsamplt.o:lsamplt.c lunus.h
lscaleim.o:lscaleim.c lunus.h
lscalelt.o:lscalelt.c lunus.h
lscalerfim.o:lscalerfim.c lunus.h
lsetped.o:lsetped.c lunus.h
lsettag.o:lsettag.c lunus.h
lshiftlt.o:lshiftlt.c lunus.h
lshiftsflt.o:lshiftsflt.c lunus.h
lshim4lt.o:lshim4lt.c lunus.h
lshimlt.o:lshimlt.c lunus.h
lsmoothim.o:lsmoothim.c lunus.h
lsmoothrf.o:lsmoothrf.c lunus.h
lsmthim.o:lsmthim.c lunus.h
lsolidlt.o:lsolidlt.c lunus.h
lspline.o:lspline.c lunus.h
lsqrtrf.o:lsqrtrf.c lunus.h
lsqsubim.o:lsqsubim.c lunus.h
lsubenvlt.o:lsubenvlt.c lunus.h
lsubim.o:lsubim.c lunus.h
lsublt.o:lsublt.c lunus.h
lsubminlt.o:lsubminlt.c lunus.h
lsubrf.o:lsubrf.c lunus.h
lsubrfim.o:lsubrfim.c lunus.h
lsubrflt.o:lsubrflt.c lunus.h
lsumim.o:lsumim.c lunus.h
lsumlt.o:lsumlt.c lunus.h
lsumresim.o:lsumresim.c lunus.h
lsumrf.o:lsumrf.c lunus.h
lsumrim.o:lsumrim.c lunus.h
lsumscim.o:lsumscim.c lunus.h
lsvtools.o:lsvtools.c lunus.h
lsymlib.o:lsymlib.c lunus.h
lsymlt.o:lsymlt.c lunus.h
lsymminlt.o:lsymminlt.c lunus.h
ltagim.o:ltagim.c lunus.h
ltaglt.o:ltaglt.c lunus.h
lthrshim.o:lthrshim.c lunus.h
lthrshlt.o:lthrshlt.c lunus.h
ltordata.o:ltordata.c lunus.h
ltranslt.o:ltranslt.c lunus.h
ltransmap.o:ltransmap.c lunus.h
lupdbd.o:lupdbd.c lunus.h
lwaveim.o:lwaveim.c lunus.h
lwindim.o:lwindim.c lunus.h
lwritecube.o:lwritecube.c lunus.h
lwritehkl.o:lwritehkl.c lunus.h
lwriteim.o:lwriteim.c lunus.h
lwritelt.o:lwritelt.c lunus.h
lwritemap.o:lwritemap.c lunus.h
lwriterf.o:lwriterf.c lunus.h
lwritesh.o:lwritesh.c lunus.h
lwritevtk.o:lwritevtk.c lunus.h
lxavgr.o:lxavgr.c lunus.h
lxavgrim.o:lxavgrim.c lunus.h
lxflt.o:lxflt.c lunus.h
lxfmask.o:lxfmask.c lunus.h
.c.o:
	$(CC) -c -o ${L}/$*.o $(CFLAGS) ${L}/$*.c
#
# Libraries:
#
libmw.a: \
	labsim.o \
	labslt.o \
	lanisolt.o \
	lanisoult.o \
	lavgim.o \
	lavgpolim.o \
	lavgrf.o \
	lavgrim_aniso.o \
	lavgrim.o \
	lavgrlt.o \
	lavgsqim.o \
	lavsqrim.o \
	lavsqrlt.o \
	lbeamim.o \
	lbuttim.o \
	lcalcrsf.o \
	lcbftool.o \
	lccrim.o \
	lccrim_helper.o \
	lccrlt.o \
	lcfim.o \
	lchbyte.o \
	lconstim.o \
	lconstlt.o \
	lconstrf.o \
	lcorrlt.o \
	lcpltmap.o \
	lcpmaplt.o \
	lcullconelt.o \
	lcullim.o \
	lculllt.o \
	lcullreslt.o \
	lcutim.o \
	ldebraggim.o \
	ldfrflt.o \
	ldfsqrlt.o \
	ldiffim.o \
	ldivlt.o \
	ldivrf.o \
	lexplt.o \
	lfft.o \
	lfillim.o \
	lfillpixim.o \
	lfreeim.o \
	lfreelt.o \
	lfrmsdim.o \
	lgausslt.o \
	lgensv.o \
	lgetanls.o \
	lgetmat.o \
	lgetovld.o \
	lgetpix.o \
	lgetpks.o \
	lgetrim.o \
	lgettag.o \
	linitim.o \
	linitlt.o \
	linitmap.o \
	linitxs.o \
	lintxdslt.o \
	lliquidcorrlt.o \
	lliquidfaclt.o \
	llowrim.o \
	lmathim.o \
	lmattools.o \
	lmedim.o \
	lmin.o \
	lminim.o \
	lminrim.o \
	lminrlt.o \
	lmirrorlt.o \
	lmodeim.o \
	lmulcfim.o \
	lmuldwflt.o \
	lmulim.o \
	lmullt.o \
	lmulrf.o \
	lmulrfim.o \
	lmulscim.o \
	lmulsclt.o \
	lnign.o \
	lnoiseim.o \
	lnormim.o \
	lnormlt.o \
	lp6mmask.o \
	lparallel.o \
	lparsecelllt.o \
	lpeakim.o \
	lpolarim.o \
	lpunch.o \
	lpunchim.o \
	lratioim.o \
	lrbminrim.o \
	lreadcbf.o \
	lreadcube.o \
	lreadhkl.o \
	lreadim.o \
	lreadim_orig.o \
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
	lrmsdim.o \
	lrotlt.o \
	lrsccmap.o \
	lrsrlt.o \
	lsamplt.o \
	lscaleim.o \
	lscalelt.o \
	lscalerfim.o \
	lsetped.o \
	lsettag.o \
	lshiftlt.o \
	lshiftsflt.o \
	lshim4lt.o \
	lshimlt.o \
	lsmoothim.o \
	lsmoothrf.o \
	lsmthim.o \
	lsolidlt.o \
	lspline.o \
	lsqrtrf.o \
	lsqsubim.o \
	lsubenvlt.o \
	lsubim.o \
	lsublt.o \
	lsubminlt.o \
	lsubrf.o \
	lsubrfim.o \
	lsubrflt.o \
	lsumim.o \
	lsumlt.o \
	lsumresim.o \
	lsumrf.o \
	lsumrim.o \
	lsumscim.o \
	lsvtools.o \
	lsymlib.o \
	lsymlt.o \
	lsymminlt.o \
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
