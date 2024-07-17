#  Called with the name of the edge node
#  Adds labels for the SMARTER Demo Application
#
#
export NODE_NAME=$1
kubectl label node $NODE_NAME smarter-fluent-bit=enabled
kubectl label node $NODE_NAME smarter-gstreamer=enabled
kubectl label node $NODE_NAME smarter-inference=enabled
kubectl label node $NODE_NAME smarter-image-detector=enabled
