# Diplomacy treaties on feature/diplomacy-2 branch by SneakBug8

The goal is to make diplomatic opinion another virtual currency that can be used to unilaterally buy things from the counterparty.

This idea was initially proposed by @peter and we can see similar mechanics with favors in EU4/Vic3 and nowadays treaties in Vic3.

The changes are as following:
- Increase/decrease relations actions are banned
- Two-way relations become unilateral opinion about the other party (between -300 and +300) representing indebtedness to the counterparty.
- Influence is integrated into the opinion: investing influence points makes the other party indebted to you (higher opinion) representing the scare of might and industry of GPs. Then GPs buy the usual sphering actions with that opinion.

To make this system controllable by the player in spots like intervening in wars we say that the other party should be indebted to the player for the player to intervene.


---

get_unilateral_relationship_by_unilateral_pair
unilateral_relationship_get_opinion

asker gives military access to the target
relationship is between (asker, target)
opinion is between (target, asker)

source sends subsidies to target
relationship (target, source)
relations go between (source, target)