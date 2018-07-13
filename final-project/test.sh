image=$1

rm -rf data/a4_digits/${image}
rm -rf data/a4_digits/${image}_dilation
rm -rf data/a4_digits/${image}_resized
mkdir data/a4_digits/${image}

echo '======================'

echo 'STEP 1: detection & digit segmentation'
bin/detect ${image}

echo '======================'

echo 'STEP 2: digit recognition'
python src/resize.py ${image}
python src/svm_test.py ${image}
