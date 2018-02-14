# copy the main op file to the pip distribution package folder
cp ../src/python/tfmongodb_ops.py tfmongodb/__init__.py

# copy the library to the distribution package folder
if [[ "$OSTYPE" == "linux-gnu" ]]; then
  cp ../cmake-build-debug/libTFMongoDB.so tfmongodb/
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cp ../cmake-build-debug/libTFMongoDB.dylib
else
  echo "Your system is currently not supported"
fi

# let setup.py build the wheel
python setup.py bdist_wheel
