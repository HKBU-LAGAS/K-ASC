# KASC

Code for the paper titled "Efficient Swing Computation for Retrieval in Large-Scale
Recommender Systems" (SIGMOD Submission ID: 642)

## Environment

- System: Oracle Linux 8.8 (64-bit)
- Compiler: gcc 8.5.0

## Datasets

| **Dataset**  | **$|\mathcal{U}|$** | **$|\mathcal{I}|$** | **$|\mathcal{E}|$** | **Item Type** |                           **Link**                           |
| :----------: | :-----------------: | :-----------------: | :-----------------: | :-----------: | :----------------------------------------------------------: |
| $MovieLens$  |        6,040        |        3,706        |      1,000,209      |     movie     |    [Download](https://grouplens.org/datasets/movielens/)     |
|  $Gowalla$   |       29,858        |       40,981        |      1,027,370      |   location    |    [Download](https://github.com/snap-research/GraphHash)    |
| $AmazonBook$ |       52,643        |       91,599        |      2,984,108      |     book      |    [Download](https://github.com/snap-research/GraphHash)    |
| $SteamGame$  |      2,567,538      |       15,474        |      6,889,728      |  video game   | [Download](https://cseweb.ucsd.edu/~jmcauley/datasets.html#steam_data) |
|    $MIND$    |       876,956       |       97,509        |     18,149,915      |     news      |            [Download](https://msnews.github.io/)             |
|   $Twitch$   |     15,524,309      |       790,100       |     234,422,289     |   streamer    | [Download](https://cseweb.ucsd.edu/~jmcauley/datasets.html#twitch) |
|   $Yambda$   |       921,023       |      8,746,689      |    1,339,219,563    |     music     |  [Download](https://huggingface.co/datasets/yandex/yambda)   |
|    $MAG$     |     10,541,560      |      2,784,240      |    1,095,315,106    |     paper     | [Download](https://figshare.com/articles/dataset/mag_scholar/12696653) |

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

