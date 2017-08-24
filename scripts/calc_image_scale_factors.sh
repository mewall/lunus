#!/bin/bash -fx

. $1

if [ -z ${work_dir+x} ]; then echo "work_dir is unset"; else echo "work_dir is set to '$work_dir'"; fi
if [ -z ${lunus_image_dir+x} ]; then echo "lunus_image_dir is unset"; else echo "lunus_image_dir is set to '$lunus_image_dir'"; fi
if [ -z ${image_prefix+x} ]; then echo "image_prefix is unset"; else echo "image_prefix is set to '$image_prefix'"; fi
if [ -z ${num_images+x} ]; then echo "num_images is unset"; else echo "num_images is set to '$num_images'"; fi

if [ ! -d $work_dir ]; then
	mkdir $work_dir
fi

cd $work_dir

if [ ! -d radial_averages ]; then
	mkdir radial_averages
fi

if [ -e $scales_output_file ]; then
    rm $scales_output_file
#    echo "Output file $scales_output_file already exists. Delete first."
    return
fi

# Reference file

reference_image_path=`printf %s/%s_%05d.img $lunus_image_dir $scale_image_prefix $reference_image_number`

# Calculate the scale factors

for (( i=1; i <= $num_images ; i++ ))

do

this_image_name=`printf %s_%05d.img $scale_image_prefix $i`

this_image_path=`printf %s/%s $lunus_image_dir $this_image_name`

scale_image_name=`printf %s_%05d.img $scale_image_prefix $i`

scale_image_path=`printf %s/%s $lunus_image_dir $scale_image_name`

lunus_image_name=`printf %s_%05d.img $lunus_image_prefix $i`

lunus_image_path=`printf %s/%s $lunus_image_dir $lunus_image_name`

scale_output=`scaleim $reference_image_path $scale_image_path $scale_inner_radius $scale_outer_radius`

echo "$i $lunus_image_name using $this_image_path scale,scale_error=$scale_output"

echo "$i $lunus_image_path $scale_output" >> $scales_output_file 

done

cd -
