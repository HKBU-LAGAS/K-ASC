## Environment

- System: Oracle Linux 8.8 (64-bit)
- Compiler: gcc 8.5.0

## Datasets

Download them from [here](https://)

## Generating queries

```shell
# Generate all item and popular item random samples
$ sh genseeds.sh
# Compute the ground truth necessary for metrics calculation using Exact algorithm
$ sh groundTruth.sh  
```

## Running

```shell
$ sh build.sh
# ./main -g ${data} -a ${algo} -r ${samples}
$ ./main -g Gowalla -a KASC -r all
```

