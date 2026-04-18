# KASC

Code for the Sigmod paper Submission Id: 642 titled "Efficient Swing Computation for Retrieval in Large-Scale
Recommender Systems"

## Environment

- System: Ubuntu 18.04.2 LTS
- Compiler: gcc 7.5.0

## Datasets

Download them from [here](https://)

## Generating queries

```python
$ cd data/
$ python genseed.py Gowalla # python data_name
```



## Running

```shell
$ sh build.sh
$ ./main -g Gowalla -a KASC
```
