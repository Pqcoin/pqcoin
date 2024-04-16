<h1 align="center">
<br/><br/>
Pqcoin Core [PQ, Ð]  
</h1>
<div align="center">


</div>

Select language: EN | [CN](./README_zh_CN.md) 

Pqcoin is a community-driven cryptocurrency. The Pqcoin Core software allows anyone to operate a node in the Pqcoin blockchain networks and uses the Scrypt hashing method and DOUBLE-SHA256 hashing method for Proof of Work. It is adapted from Bitcoin Core and other cryptocurrencies.

For information about the default fees used on the Pqcoin network, please
refer to the [fee recommendation](./doc/fee-recommendation.md).

**Website:** [pqcoin.net](https://www.pqcoin.net)

## Usage 💻

To start your journey with Pqcoin Core, see the [installation guide](./INSTALL.md) and the [getting started](./doc/getting-started.md) tutorial.

The JSON-RPC API provided by Pqcoin Core is self-documenting and can be browsed with `pqcoin-cli help`, while detailed information for each command can be viewed with `pqcoin-cli help <command>`. Alternatively, see the [Bitcoin Core documentation](https://developer.bitcoin.org/reference/rpc/) - which implement a similar protocol - to get a browsable version.

### Such ports

Pqcoin Core by default uses port `22556` for peer-to-peer communication that
is needed to synchronize the "mainnet" blockchain and stay informed of new
transactions and blocks. Additionally, a JSONRPC port can be opened, which
defaults to port `22555` for mainnet nodes. It is strongly recommended to not
expose RPC ports to the public internet.

| Function | mainnet | testnet | regtest |
| :------- | ------: | ------: | ------: |
| P2P      |   22556 |   44556 |   18444 |
| RPC      |   22555 |   44555 |   18332 |

## Ongoing development - Moon plan 🌒

Pqcoin Core is an open source and community driven software. The development
process is open and publicly visible; anyone can see, discuss and work on the
software.

Main development resources:

* [GitHub Projects](https://github.com/pqcoin/pqcoin/projects) is used to
  follow planned and in-progress work for upcoming releases.
* [GitHub Discussion](https://github.com/pqcoin/pqcoin/discussions) is used
  to discuss features, planned and unplanned, related to both the development of
  the Pqcoin Core software, the underlying protocols and the PQ asset.  
* [Pqcoindev subreddit](https://www.reddit.com/r/pqcoindev/)

### Version strategy
Version numbers are following ```major.minor.patch``` semantics.

### Branches
There are 3 types of branches in this repository:

- **master:** Stable, contains the latest version of the latest *major.minor* release.
- **maintenance:** Stable, contains the latest version of previous releases, which are still under active maintenance. Format: ```<version>-maint```
- **development:** Unstable, contains new code for planned releases. Format: ```<version>-dev```

*Master and maintenance branches are exclusively mutable by release. Planned*
*releases will always have a development branch and pull requests should be*
*submitted against those. Maintenance branches are there for **bug fixes only,***
*please submit new features against the development branch with the highest version.*

## Contributing 🤝

If you find a bug or experience issues with this software, please report it
using the [issue system](https://github.com/pqcoin/pqcoin/issues/new?assignees=&labels=bug&template=bug_report.md&title=%5Bbug%5D+).

Please see [the contribution guide](CONTRIBUTING.md) to see how you can
participate in the development of Pqcoin Core. There are often
[topics seeking help](https://github.com/pqcoin/pqcoin/labels/help%20wanted)
where your contributions will have high impact and get very appreciation. wow.

## Communities 🚀🍾

You can join the communities on different social media.
To see what's going on, meet people & discuss, find the latest meme, learn
about Pqcoin, give or ask for help, to share your project.

Here are some places to visit:

* [Pqcoin subreddit](https://www.reddit.com/r/pqcoin/)
* [Pqducation subreddit](https://www.reddit.com/r/pqducation/)
* [Discord](https://discord.gg/pqcoin)
* [Pqcoin Twitter](https://twitter.com/PQ_coin)

## Very Much Frequently Asked Questions ❓

Do you have a question regarding Pqcoin? An answer is perhaps already in the
[FAQ](doc/FAQ.md) or the
[Q&A section](https://github.com/pqcoin/pqcoin/discussions/categories/q-a)
of the discussion board!

## License - Much license ⚖️
Pqcoin Core is released under the terms of the MIT license. See
[COPYING](COPYING) for more information or see
[opensource.org](https://opensource.org/licenses/MIT)
