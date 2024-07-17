#  Called with the name of the edge node
#  Removes labels for the SMARTER Demo Application
#
#
export NODE_NAME=$1
kubectl label node $NODE_NAME smarter-fluent-bit-
kubectl label node $NODE_NAME smarter-gstreamer-
kubectl label node $NODE_NAME smarter-inference-
kubectl label node $NODE_NAME smarter-image-detector-
