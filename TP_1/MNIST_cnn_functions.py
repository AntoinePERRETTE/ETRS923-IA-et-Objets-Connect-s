#MNIST: Take II
import tensorflow as tf

#This specifies the weights for either fully connected or convolutional layers of the network. They are initialized randomly using a truncated normal distribution with a standard deviation of   0.1. This sort of initialization with a random normal distribution that is truncated at the tails is pretty common and generally produces good results
def weight_variable(shape):
    initial = tf.random.truncated_normal(shape, stddev=0.1)
    return tf.Variable(initial)

#This defines the bias elements in either a fully connected or a convolutional layer. These are all initialized with the constant value of 0.1.
def bias_variable(shape):
    initial = tf.constant(0.1, shape=shape)
    return tf.Variable(initial)

#This specifies the convolution we will typically use. A full convolution (no skips) with an output the same size as the input.
#This function is used in the convolutional layer above
def conv2d(x, W):
    return tf.nn.conv2d(x, W, strides=[1, 1, 1, 1], padding='SAME')

#This sets the max pool to half the size across the height/width dimensions, and in total a quarter the size of the feature map.
#Execute the Pooling between convolutional layers to reduce the size of the data
def max_pool_2x2(x):
    return tf.nn.max_pool(x, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding='SAME')

#This is the actual layer we will use. Linear convolution as defined in conv2d, with a bias, followed by the ReLU nonlinearity.
def conv_layer(input, shape):
    W = weight_variable(shape)
    b = bias_variable([shape[3]])
    return tf.nn.relu(conv2d(input, W) + b)

#A standard full layer with a bias. Notice that here we didn’t add the ReLU. This allows us to use the same layer for the final output, where we don’t need the non-linear part.
#Fully Connected Layer, with linear activation function.
def full_layer(input, size):
    in_size = int(input.get_shape()[1])
    W = weight_variable([in_size, size])
    b = bias_variable([size])
    return tf.matmul(input, W) + b