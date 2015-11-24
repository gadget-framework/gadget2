
# A Brief Guide to a Simple Gadget Example


This is a guide to a Gadget model example that is currently available
for the Gadget software. This example is intended to show how the
basics of how Gadget works, and as such some elements of the model
have been skipped for simplicity. The stock used in this example is
haddock in Icelandic waters, and the data files for this example can
be downloaded from the Hafro ftp site, and extracts from these data
files are included in this guide.

It is recommended that this guide is read in conjunction with the
Gadget User Guide, which is also available from the Hafro ftp site. As
explained in the user guide, comments in the data files are denoted by
a semi-colon '';'', and parameters that can be optimised by Gadget are
denoted by a hash ''#''. 

The run this example, the user should ensure that a copy of the Gadget executable has been compiled and placed in the directory containing the model files, and then run Gadget using the following command:

	gadget -s -i refinputfile


## Main File

The main input file is called ''main''. This file contains links to other files which make up the Gadget model. There is no data declared in this file, only links to other files.

	;
	; Main file for the haddock example
	;
	timefile    time        ; specifying years 1978-2006
	areafile    area        ; specifying only one area
	printfiles  printfile   ; specifying the printer classes
	;
	[stock]          ; the description of the stock data
	stockfiles       had
	;
	[tagging]        ; the description of the tagging data
	; currently no tagging experiments in this haddock model
	;
	[otherfood]      ; the description of the otherfood data
	; currently no otherfood in this haddock model
	;
	[fleet]          ; the description of the fleet data
	fleetfiles       fleet
	;
	[likelihood]     ; the description of the likelihood data
	likelihoodfiles  likelihood

The first 2 lines of the main file for this haddock example list the
files that define the timesteps and area to be used for the
example. The next line lists the file that is to be used to specify
the printing that is required - that is output from the modelled data,
not output from the model parameters.

In the [stock] section, there is one file listed which describes the
haddock stock to be used for this example. There are then sections for
the tagging experiments and the otherfood, which are blank for this
example. The [fleet] section lists the file that is required to define
the fleets for this example. Finally there is the [likelihood] section
which lists the file that defines the likelihood components for this
example.


## Time File

The time file defines the timesteps to be used by Gadget for this
example. In this case, Gadget is to run a model from 1978 through to
2006, with 4 equal timesteps in each year.

	;
	; Time file for this haddock model
	;
	firstyear       1978
	firststep       1
	lastyear        2006
	laststep        4
	notimesteps     4 3 3 3 3


## Area File

The area file defines the area data to be used by Gadget for this
example. In this example, a single area has been defined, with a
constant temperature for the duration of the model.

	;
	; Area file for this haddock model
	;
	areas  1
	size   200000
	temperature
	;
	; year  step  area  temperature
	1978    1     1     5
	1978    2     1     5
	1978    3     1     5
	1978    4     1     5
	1979    1     1     5
	1979    2     1     5
	1979    3     1     5
	1979    4     1     5
	1980    1     1     5
	1980    2     1     5
	1980    3     1     5
	1980    4     1     5
	1981    1     1     5
	1981    2     1     5
	1981    3     1     5
	1981    4     1     5
	1982    1     1     5
	1982    2     1     5
	...
	2004    3     1     5
	2004    4     1     5
	2005    1     1     5
	2005    2     1     5
	2005    3     1     5
	2005    4     1     5
	2006    1     1     5
	2006    2     1     5
	2006    3     1     5
	2006    4     1     5

## Aggregation Files

There are a number of simple aggregation files that are required for
this example.

### Age Aggregation Files

There are 2 age aggregation files - one that lists the possible ages individually (ie. no aggregation) and one that groups all the ages together into one age group. These age aggregation files also define the text labels that are to be used when inputting and outputting the data for this example.

	;
	; Age aggregation file - no aggregation
	;
	age1    1
	age2    2
	age3    3
	age4    4
	age5    5
	age6    6
	age7    7
	age8    8
	age9    9
	age10   10


	;
	; Age aggregation file - all ages aggregated together
	;
	allages    1  2  3  4  5  6  7  8  9  10

### Area Aggregation File

Although there is only one area in this example, it is still necessary
to define a area aggregation file. This is because it defines a text
label that is to be used when inputting and outputting the data for
this example.

	;
	; Area aggregation file - only one area
	;
	allareas   1

### Length Aggregation Files

There are a total of 8 length aggregation files for this example. One
aggregation file aggregates the stock into 2cm length groups (by
combining the 1cm length groups that are declared for the stock), and
a second file aggregates all the length groups together into one
length group. There are also 6 aggregation files corresponding to the
6 survey index likelihood components that are declared in the
likelihood file, which aggregate the stock into one or more 5cm length
groups.

	;
	; Length aggregation file - 2cm length groups
	;
	len1     4.5     6.5
	len2     6.5     8.5
	len3     8.5     10.5
	len4     10.5    12.5
	len5     12.5    14.5
	len6     14.5    16.5
	len7     16.5    18.5
	len8     18.5    20.5
	len9     20.5    22.5
	len10    22.5    24.5
	len11    24.5    26.5
	len12    26.5    28.5
	len13    28.5    30.5
	len14    30.5    32.5
	len15    32.5    34.5
	len16    34.5    36.5
	len17    36.5    38.5
	len18    38.5    40.5
	len19    40.5    42.5
	len20    42.5    44.5
	len21    44.5    46.5
	len22    46.5    48.5
	len23    48.5    50.5
	...
	len34    70.5    72.5
	len35    72.5    74.5
	len36    74.5    76.5
	len37    76.5    78.5
	len38    78.5    80.5
	len39    80.5    82.5
	len40    82.5    84.5
	len41    84.5    86.5
	len42    86.5    88.5
	len43    88.5    90.5


	;
	; Length aggregation file - all lengths aggregated together
	;
	all      4.5     90.5


	;
	; Length aggregation file for survey indices
	;
	mlen10   7.5     12.5


	;
	; Length aggregation file for survey indices
	;
	mlen15   12.5    17.5


	;
	; Length aggregation file for survey indices
	;
	mlen20   17.5    22.5


	;
	; Length aggregation file for survey indices
	;
	mlen25   22.5    27.5
	mlen30   27.5    32.5
	mlen35   32.5    37.5
	mlen40   37.5    42.5
	mlen45   42.5    47.5


	;
	; Length aggregation file for survey indices
	;
	mlen50   47.5    52.5
	mlen55   52.5    57.5
	mlen60   57.5    62.5


	;
	; Length aggregation file for survey indices
	;
	mlen65   62.5    67.5
	mlen70   67.5    72.5
	mlen75   72.5    77.5

## Stock File

The stock file contains the various parameters that define the stock
to be used in the Gadget model. The first section of this file gives
the minimum and maximum age and length of the stock and the location
of a reference weight file that specifies a reference weight for each
length group for the stock.

The next section of this file cover the parameters required for the
growth of the stock. The growth function used in this example is an
expanded form of the Von Bertalanffy growth function, split so that
the increase in weight is calculated first, and then the change in
weight is used to calculate a change in length, as shown in equation 1
and equation 4 below:

* $\Delta W_i = \Delta t q_0 e^{q_1T}\left(\left( \frac{W_i}{q_2}
\right)^{q_4} - \left(\frac{W_i}{q_3} \right)^{q_5} \right)$
* $r = \frac{W - \left( p_{0} + p_{8} \left( p_{1} + p_{2}p_{8} \right)
\right) W_{ref}}{W}$
* $f(x) = \begin{cases}0 &
\textrm{if $p_{3} + p_{4}x \leq 0$} \\ p_......$p_{3} + p_{4}x \geq
p_{5}$} \\ p_{3} + p_{4}x & \textrm{otherwise} \end{cases}$
* $\Delta L_i = \frac{\Delta W_i} {p_{6} p_{7} l^{p_{7} -1}} f(r)$ (4)

where:
* < Δt 	 is the length of the timestep
* < T 	 is the temperature
* < Wref 	 is the reference weight

For this example, q1 is set to zero, removing the temperature
dependance from the equation, and q2 is set equal to q3, further
simplifying the equation. Equations 2 and 3 introduce the concept of
starvation to the Gadget model, by using a function of the weight and
the reference weight when calculating the length increase due to the
growth. For this example, p0 is set to one and p8 to zero, which
considerably simplifies equation 2. To simplify the growth function
further, it is possible to remove the concept of starvation from
equation 3 by setting p3 to zero and p4 and p5 to one. Once Gadget has
calculated the mean increase in weight and length, this increase is
then distributed amongst the length groups using a beta-binomial
distribution that is defined by the parameters beta and
maxlengthgroupgrowth.

The stock file then defines the age based natural mortality that is to
be applied to the stock. The next section of the stock file defines
whether the stock acts as a prey or a predator, and specifies the
initial conditions, which are used to calculate the stock that exists
at the beginning of the first timestep. There then follows sections
used to describe how the stock would migrate, mature, move, recruit,
spawn and stray, which are mostly unused for the example. However, the
stock does have recruits to ensure that it doesn't die out, which are
defined in the recruitment file.

	;
	; Haddock stock file for this haddock model
	;
	stockname       had
	livesonareas    1
	minage          1
	maxage          10
	minlength       4.5
	maxlength       90.5
	dl              1
	refweightfile   had.refweights
	;
	; the growth and consumption can be calculated on a finer scale
	; so the length groups for this are specified here
	;
	growthandeatlengths   len.agg
	;
	; information about the growth of the stock
	;
	doesgrow              1
	growthfunction        weightvb
	;
	; the parameters required for this growth function are
	;
	wgrowthparameters     #grq0  0  #grq2  #grq2  0.666  1
	lgrowthparameters     1  0  0  1  2.2  1.4  8.85e-6  3.0257  0
	;
	beta                  #bbeta
	maxlengthgroupgrowth  20
	;
	; information about the natural mortality of the stock
	; age             1   2    3   4   5   6   7   8   9   10
	naturalmortality  0.5 0.35 0.2 0.2 0.2 0.2 0.2 0.3 0.4 0.7
	;
	; information about the stock acting as a prey
	;
	iseaten           1 ; the fleet is considered a predator
	preylengths       len.agg
	energycontent     1
	;
	; information about the stock acting as a predator
	;
	doeseat           0
	;
	; information about the initial conditions of the stock
	;
	initialconditions
	minage            1
	maxage            10
	minlength         4.5
	maxlength         90.5
	normalcondfile    had.init
	;
	; information about the migration of the stock
	;
	doesmigrate       0
	;
	; information about the maturation of the stock
	;
	doesmature        0
	;
	; information about the movement of the stock
	;
	doesmove          0
	;
	; information about the recruitment to the stock
	;
	doesrenew         1
	minlength         4.5
	maxlength         28.5
	normalparamfile   had.rec
	;
	; information about the spawning of the stock
	;
	doesspawn         0
	;
	; information about the straying of the stock
	;
	doesstray         0

## Reference Weight File

The reference weight file gives a reference length-weight relationship
for the stock in this example. This is used to generate the entries in
the age-length cells for the initial conditions, and to modify the
growth of the stock after starvation (so that the growth results in an
increase in weight not length for the underweight fish). This files
simply lists a ''reference'' mean weight for each length group for the
stock.

	;
	; Reference length-weight relationship for this haddock model
	;
	;length weight
	5       0.001307
	6       0.002154
	7       0.003285
	8       0.004735
	9       0.006538
	10      0.008725
	11      0.011328
	12      0.013848
	13      0.017795
	14      0.021701
	15      0.025792
	16      0.030797
	17      0.036149
	18      0.045531
	19      0.052379
	20      0.064773
	21      0.080277
	22      0.092542
	23      0.104681
	24      0.116645
	25      0.13377
	26      0.150614
	...
	81      4.753127
	82      4.918877
	83      5.088293
	84      5.261409
	85      5.438262
	86      5.618887
	87      5.803317
	88      5.99159
	89      6.183739
	90      6.3798

## Initial Conditions File

The initial conditions file gives a Normal distribution for each
area/age group combination. This will be used by Gadget to construct
an initial population of 10,000 fish in each area/age group, with the
length groups specified by a mean length and standard deviation. The
mean weight for the length groups of the initial population is
calculated by multiplying the reference weight by the condition
factor. To change from a population with 10,000 fish in each area/age
group to the initial population used in the model, each age group is
multiplied by an area weighting factor and an age weighting factor, as
specified in the initial conditions file.

	;
	; Initial conditions file for this haddock model
	;
	; age  area  agemultiplier     areamult  meanlength  standdev  condition
	1      1     0                 100       16.41203    2.247188  1
	2      1    (* 10000 #inage2)  100       27.15520    2.898219  1
	3      1    (* 6065.3 #inage3) 100       36.98713    4.070510  1
	4      1    (* 3678.8 #inage4) 100       43.77545    4.927558  1
	5      1    (* 2231.3 #inage5) 100       49.43773    5.540416  1
	6      1    (* 1353.4 #inage6) 100       53.76334    5.807182  1
	7      1    (* 820.8 #inage7)  100       58.64396    6.023261  1
	8      1    (* 497.9 #inage8)  100       66.10526    8.00      1
	9      1    (* 302 #inage9)    100       60.88235    9.00      1
	10     1    (* 10 #inage9)     100       63.00       9.00      1

## Recruitment File

The recruitment file defines the number of the recruits that are to be
added to the stock, along with information about the age, length and
weight of these recruits. The number of these recruits is given, for
each timestep/area combination, in units of 10,000 fish. The age is
specified as the minimum age of the stock.

These recruits are defined as a simple length based stock, with a
Normal distribution around a mean length and standard deviation of the
length given in the input file. The mean weight of the recruits is
then calculated from the standard weight-length relationship, given in
equation 5 below: $\displaystyle W = \alpha L ^\beta$ (5)

Note that in this example, the recruits are assumed to have the same
weight and length distribution in each year. The number of recruits
for years that there is data available (1978 - 1999) are parameters
that the optimiser can adjust to try to get a better fit between the
modelled data and the input data, where as for future years when there
is no data available it is assumed that there is a constant number of
recruits.

	;
	; Recruitment data for this haddock model
	;
	; year  step  area  age  number          meanlen  stddev alpha    beta
	1978    1     1     1   (* 1000 #rec78)  16.41    2.25   8.85e-6  3.0257
	1979    1     1     1   (* 1000 #rec79)  16.41    2.25   8.85e-6  3.0257
	1980    1     1     1   (* 1000 #rec80)  16.41    2.25   8.85e-6  3.0257
	1981    1     1     1   (* 1000 #rec81)  16.41    2.25   8.85e-6  3.0257
	1982    1     1     1   (* 1000 #rec82)  16.41    2.25   8.85e-6  3.0257
	1983    1     1     1   (* 1000 #rec83)  16.41    2.25   8.85e-6  3.0257
	1984    1     1     1   (* 1000 #rec84)  16.41    2.25   8.85e-6  3.0257
	1985    1     1     1   (* 1000 #rec85)  16.41    2.25   8.85e-6  3.0257
	1986    1     1     1   (* 1000 #rec86)  16.41    2.25   8.85e-6  3.0257
	1987    1     1     1   (* 1000 #rec87)  16.41    2.25   8.85e-6  3.0257
	1988    1     1     1   (* 1000 #rec88)  16.41    2.25   8.85e-6  3.0257
	1989    1     1     1   (* 1000 #rec89)  16.41    2.25   8.85e-6  3.0257
	1990    1     1     1   (* 1000 #rec90)  16.41    2.25   8.85e-6  3.0257
	1991    1     1     1   (* 1000 #rec91)  16.41    2.25   8.85e-6  3.0257
	1992    1     1     1   (* 1000 #rec92)  16.41    2.25   8.85e-6  3.0257
	1993    1     1     1   (* 1000 #rec93)  16.41    2.25   8.85e-6  3.0257
	1994    1     1     1   (* 1000 #rec94)  16.41    2.25   8.85e-6  3.0257
	1995    1     1     1   (* 1000 #rec95)  16.41    2.25   8.85e-6  3.0257
	1996    1     1     1   (* 1000 #rec96)  16.41    2.25   8.85e-6  3.0257
	1997    1     1     1   (* 1000 #rec97)  16.41    2.25   8.85e-6  3.0257
	1998    1     1     1   (* 1000 #rec98)  16.41    2.25   8.85e-6  3.0257
	1999    1     1     1   (* 1000 #rec99)  16.41    2.25   8.85e-6  3.0257
	2000    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2001    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2002    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2003    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2004    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2005    1     1     1    9000            16.41    2.25   8.85e-6  3.0257
	2006    1     1     1    9000            16.41    2.25   8.85e-6  3.0257


## Fleet File

The fleet file defines the fleets that are present in the Gadget
model. The fleets are defined by specifying the fleet type, name, area
and length groups (which in this example are set to the minimum and
maximum lengths of the stock). The fleets also have a suitability
function, that describes how likely it is that the fleet will catch
fish of a given length. The suitability function used is an
exponential suitability function, given in equation 6 below:
$\displaystyle S(l,L) = { \frac{\delta}{1 + e^{- \alpha - \beta l -
\gamma L}}}$ (6)

where:
< l 	 is the length of the prey
< L 	 is the length of the predator

Note that in this example, <γ	 is set to 0 which removes any
dependance on the length of the predator, and <δ	 is always set
to 1. <α	 and <β	 are parameters that the optimiser can adjust to try
to get a better fit between the modelled data and the input data.

There are 3 fleets defined in this example. The commercial fleet
(''comm'') covers all the commercial fishing activity, and all the
available landings data is specified in the data file. The survey
fleet (''survey'') covers all the government survey activity, and this
fleet is assumed to land a constant amount of fish for all the years
in the model. The third fleet (''future'') covers all the predicted
commercial fishing activity from mid 1999 (when the commercial landing
data stops being available) to the end of the models timesteps.

	;
	; Fleet data for this haddock model
	;
	; Details for the commercial fleet
	;
	[component]
	totalfleet      comm            ; fleet name
	livesonareas    1               ; areas for the fleet
	suitability                     ; alpha    beta     gamma  delta
	had    function exponential       #acomm   #bcomm   0      1
	amount          fleet.data      ; where the catch data is stored
	;
	; Details for the survey fleet
	;
	[component]
	totalfleet      survey          ; fleet name
	livesonareas    1               ; areas for the fleet
	suitability
	had    function exponential       #asur    #bsur    0      1
	amount          fleet.data      ; where the catch data is stored
	;
	; Details for the predicted fleet
	;
	[component]
	linearfleet     future          ; fleet name
	livesonareas    1               ; areas for the fleet
	multiplicative  #mult           ; scaling factor
	suitability
	had    function exponential       #acomm   #bcomm   0      1
	amount          fleet.predict   ; where the catch data is stored

## Fleet Data Files

The 2 fleet data files contain details of the landings made in each
timestep/area/fleet combination for the fleets that have been declared
in the main fleet file. The first data file is a list of the total
weight of the landing data currently available (ie. all the survey
data and the commercial landings data up to the first timestep of
1999) for each timestep/area/fleet combination. The second data file
contains a list of the ratios to be used when calculating the amount
that the fleet will catch, for the timestep/area combinations when
commercial fleet effort is required in the future and no landings data
is available (ie. from the second timestep of 1999).

	;
	; Fleet catch data in kilos for this haddock model
	;
	; year  step  area  fleet   amount
	1978    1     1     comm    8444000
	1978    2     1     comm    14834000
	1978    3     1     comm    9985000
	1978    4     1     comm    10184000
	1979    1     1     comm    10753000
	1979    2     1     comm    18893000
	1979    3     1     comm    12717000
	1979    4     1     comm    12971000
	1980    1     1     comm    9933000
	1980    2     1     comm    17451000
	1980    3     1     comm    11747000
	1980    4     1     comm    11981000
	1981    1     1     comm    12352000
	1981    2     1     comm    21701000
	1981    3     1     comm    14608000
	1981    4     1     comm    14899000
	1982    1     1     comm    13023000
	1982    2     1     comm    22880000
	1982    3     1     comm    15401000
	1982    4     1     comm    15708000
	1983    1     1     comm    15853000
	1983    2     1     comm    25218000
	...
	1997    2     1     survey  30000
	1998    2     1     survey  30000
	1999    2     1     survey  30000
	2000    2     1     survey  30000
	2001    2     1     survey  30000
	2002    2     1     survey  30000
	2003    2     1     survey  30000
	2004    2     1     survey  30000
	2005    2     1     survey  30000
	2006    2     1     survey  30000
	

	;
	; Predicted fleet effort for this haddock model
	;
	; year  step  area  fleet   effort
	1999    2     1     future  1
	1999    3     1     future  1
	1999    4     1     future  1
	2000    1     1     future  1
	2000    2     1     future  1
	2000    3     1     future  1
	2000    4     1     future  1
	2001    1     1     future  1
	2001    2     1     future  1
	2001    3     1     future  1
	2001    4     1     future  1
	2002    1     1     future  1
	2002    2     1     future  1
	2002    3     1     future  1
	2002    4     1     future  1
	2003    1     1     future  1
	2003    2     1     future  1
	2003    3     1     future  1
	2003    4     1     future  1
	2004    1     1     future  1
	2004    2     1     future  1
	2004    3     1     future  1
	2004    4     1     future  1
	2005    1     1     future  1
	2005    2     1     future  1
	2005    3     1     future  1
	2005    4     1     future  1
	2006    1     1     future  1
	2006    2     1     future  1
	2006    3     1     future  1
	2006    4     1     future  1


## Likelihood File

The likelihood file defines the various likelihood components that
will be used to compare the data from within the Gadget model with
external data. Each likelihood component calculates a ''goodness of
fit'' between the 2 sets of data to give a a likelihood score, and
there is then a weighted sum of these likelihood scores to give an
overall likelihood score, which can be minimised if Gadget is
performing an optimising run.

In this example, there are a total of 14 likelihood components defined
to test the goodness of fit between the 2 sets of data. These are
''BoundLikelihood'', ''Understocking'', 2 ''CatchStatistics'', 4
''CatchDistribution'' and 6 ''SurveyIndices'' components.
BoundLikelihood

The BoundLikelihood component is used to apply a penalty weight to any
parameter that goes outside the bounds during the optimising
process. Applying this penalty weight will force the parameter away
from the bounds and back into the range of numbers that have been
specified in the parameter file.  Understocking

The Understocking component is used to apply a penalty whenever there
has been overconsumption by predators (in this case the fleets), and
there is insufficient stock for the predator to consume. In this
example this penalty is the sum of squares of the understocking that
has occurred in the model.  CatchStatistics

The CatchStatistics components are used to compare biological data
sampled from the model with that sampled from landings data for the
fleets. In this example there are 2 comparisons, one for data from the
commercial fleet and one for the survey fleet. In each case a weighted
sum of squares of the mean length at age is used to calculate the
goodness of fit between the 2 sets of data. The data which will be
compared to the results from within the Gadget model are given in the
2 data files that are specified.  CatchDistribution

The CatchDistribution components are used to compare distribution data
sampled from the model with that sampled from landings data for the
fleets. In this example there are 2 comparisons (one for the
commercial fleet, one for the survey fleet) with the data aggregated
into length groups and a further 2 comparisons with the data
aggregated into age-length groups. In each case a multinomial function
is used to calculate the goodness of fit between the 2 sets of
data. The data which will be compared to the results from within the
Gadget model are given in the 4 data files that are specified.
SurveyIndices

The SurveyIndices components are used to compare stock indices
calculated within the Gadget model to indices calculated from a
standardized survey for that stock. In this example the survey indices
calculated are based on a length group survey, and there are 6
comparisons for 6 different length group aggregations, defined in the
various length aggregation files. The index calculated in the model
will be compared to the index that is specified in the data file,
using a log linear regression line with the slope fixed and the
intercept estimated within the model.

	;
	; Likelihood file for this haddock model
	;
	; first specify the likelihood bounds
	[component]
	name               bounds            ; likelihood component name
	weight             10                ; weight for component
	type               penalty           ; type of component
	datafile           penaltyfile       ; data file for this component
	;
	; now specify when to check for understocking
	;
	[component]
	name               understocking     ; likelihood component name
	weight             1e-12             ; weight for component
	type               understocking     ; type of component
	;
	; the mean length statistics data
	;
	[component]
	name               meanl.sur         ; likelihood component name
	weight             2e-6              ; weight for component
	type               catchstatistics   ; type of component
	datafile           had.meanle.sur    ; data file for this component
	function           lengthgivenvar    ; function type
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         age.agg           ; age aggregation file
	fleetnames         survey            ; source of fleet data
	stocknames         had               ; source of stock data
	;
	[component]
	name               meanl.catch       ; likelihood component name
	weight             0.3e-6            ; weight for component
	type               catchstatistics   ; type of component
	datafile           had.meanle.catch  ; data file for this component
	function           lengthgivenvar    ; function type
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         age.agg           ; age aggregation file
	fleetnames         comm              ; source of fleet data
	stocknames         had               ; source of stock data
	;
	; the length distribution data
	;
	[component]
	name               ldist.sur         ; likelihood component name
	weight             0.05e-6           ; weight for component
	type               catchdistribution ; type of component
	datafile           had.ldist.sur     ; data file for this component
	function           multinomial       ; function type
	epsilon            20                ; used when outcome is improbable
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         allage.agg        ; age aggregation file
	lenaggfile         len.agg           ; length aggregation file
	fleetnames         survey            ; source of fleet data
	stocknames         had               ; source of stock data
	;
	[component]
	name               ldist.catch       ; likelihood component name
	weight             3e-6              ; weight for component
	type               catchdistribution ; type of component
	datafile           had.ldist.catch   ; data file for this component
	function           multinomial       ; function type
	epsilon            20                ; used when outcome is improbable
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         allage.agg        ; age aggregation file
	lenaggfile         len.agg           ; length aggregation file
	fleetnames         comm              ; source of fleet data
	stocknames         had               ; source of stock data
	;
	; the age-length distribution data
	;
	[component]
	name               alkeys.sur        ; likelihood component name
	weight             7e-6              ; weight for component
	type               catchdistribution ; type of component
	datafile           had.alkeys.sur    ; data file for this component
	function           multinomial       ; function type
	epsilon            20                ; used when outcome is improbable
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         age.agg           ; age aggregation file
	lenaggfile         len.agg           ; length aggregation file
	fleetnames         survey            ; source of fleet data
	stocknames         had               ; source of stock data
	;
	[component]
	name               alkeys.catch      ; likelihood component name
	weight             2e-6              ; weight for component
	type               catchdistribution ; type of component
	datafile           had.alkeys.catch  ; data file for this component
	function           multinomial       ; function type
	epsilon            20                ; used when outcome is improbable
	areaaggfile        allarea.agg       ; area aggregation file
	ageaggfile         age.agg           ; age aggregation file
	lenaggfile         len.agg           ; length aggregation file
	fleetnames         comm              ; source of fleet data
	stocknames         had               ; source of stock data
	;
	; the survey index data
	;
	[component]
	name          si10                   ; likelihood component name
	weight        70e-4                  ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si10len.agg            ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed
	;
	[component]
	name          si15                   ; likelihood component name
	weight        100e-4                 ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si15len.agg            ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed
	;
	[component]
	name          si20                   ; likelihood component name
	weight        100e-4                 ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si20len.agg            ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed
	;
	[component]
	name          si25to45               ; likelihood component name
	weight        100e-4                 ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si2545len.agg          ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed
	;
	[component]
	name          si50to60               ; likelihood component name
	weight        100e-4                 ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si5060len.agg          ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed
	;
	[component]
	name          si65to75               ; likelihood component name
	weight        70e-4                  ; weight for component
	type          surveyindices          ; type of component
	datafile      had.surveyindex        ; data file for this component
	sitype        lengths                ; survey index type
	areaaggfile   allarea.agg            ; area aggregation file
	lenaggfile    si6575len.agg          ; length aggregation file
	stocknames    had                    ; source of stock data
	fittype       fixedslopeloglinearfit ; type of data fit
	slope         1                      ; slope is fixed

## Penalty File

The penalty file contains the likelihood penalty that is to be applied
when any of the parameters goes outside its bound, defined in the
parameter input file. For this example, only a default setting is
given which will be applied to each parameter that goes outside a
bound.

	;
	; Penalty file for this haddock model
	; This file lists the penalties applied to the variables
	; when the value reaches the bound specified in the inputfile
	;
	; switch   power  lowerW   upperW
	default    2      10000    10000 ; default setting

## Mean Length Files

The 2 mean length data files contain the number of samples, and the
mean length and standard deviation of the length for these samples, in
each timestep/area/age combination for the 2 fleets. For this
likelihood component there is no area or age aggregation, as defined
by the aggregation files declared in the main likelihood file.

The likelihood function that is used to compare the data from these
files with the corresponding data from the model is a weighted sum of
squares of the mean length, given in equation 7 below: $\displaystyle
\ell = \sum_{\it time}\sum_{\it areas}\sum_{\it ages}
\Big(\frac{(x-\mu)^2} {s^2} N\Big)$ (7)

where:
< x 	 is the sample mean length from the data
< μ 	 is the mean length calculated from the model
< s 	 is the standard deviation of the length from the data
< N 	 is the sample size

	;
	; Mean length data for the survey catch for this haddock model
	;
	; year   step  area      age    number  mean   stddev
	1989     2     allareas  age1   60      15.35  2.0
	1989     2     allareas  age2   124     28.01  3.7
	1989     2     allareas  age3   238     36.14  6.2
	1989     2     allareas  age4   829     45.64  5.0
	1989     2     allareas  age5   336     54.41  5.5
	1989     2     allareas  age6   106     63.42  6.3
	1989     2     allareas  age7   8       67.88  7.1
	1989     2     allareas  age8   8       72.88  6.6
	1989     2     allareas  age9   4       70.75  5.8
	1989     2     allareas  age10  3       75     5.6
	1990     2     allareas  age1   235     15.63  2.0
	1990     2     allareas  age2   227     27.51  3.7
	1990     2     allareas  age3   192     37.13  6.2
	1990     2     allareas  age4   267     44.58  5.0
	1990     2     allareas  age5   620     51.01  5.5
	1990     2     allareas  age6   299     59.47  6.3
	1990     2     allareas  age7   38      67.29  7.1
	1990     2     allareas  age8   9       72.11  6.6
	1990     2     allareas  age9   2       71     5.8
	1990     2     allareas  age10  0       0      5.6
	1991     2     allareas  age1   350     15.85  2.0
	1991     2     allareas  age2   808     27.49  3.7
	...
	1999     2     allareas  age1   310     14.67  2.0
	1999     2     allareas  age2   312     27.63  3.7
	1999     2     allareas  age3   148     36.2   6.2
	1999     2     allareas  age4   174     44.36  5.0
	1999     2     allareas  age5   24      50.21  5.5
	1999     2     allareas  age6   15      55.53  6.3
	1999     2     allareas  age7   4       63.25  7.1
	1999     2     allareas  age8   6       60.33  6.6
	1999     2     allareas  age9   1       69     5.8
	1999     2     allareas  age10  0       0      5.6


	;
	; Mean length data for the commercial catch for this haddock model
	;
	; year   step  area      age    number  mean   stddev
	1979     1     allareas  age1   0       0      2.0
	1979     1     allareas  age2   0       0      3.7
	1979     1     allareas  age3   0       0      6.2
	1979     1     allareas  age4   7       55.43  5.0
	1979     1     allareas  age5   48      58.92  5.5
	1979     1     allareas  age6   96      64.2   6.3
	1979     1     allareas  age7   36      68.61  7.1
	1979     1     allareas  age8   4       73.5   6.6
	1979     1     allareas  age9   1       73     5.8
	1979     1     allareas  age10  0       0      5.6
	1979     2     allareas  age1   0       0      2.0
	1979     2     allareas  age2   0       0      3.7
	1979     2     allareas  age3   13      42.15  6.2
	1979     2     allareas  age4   73      51.44  5.0
	1979     2     allareas  age5   224     58.56  5.5
	1979     2     allareas  age6   258     62.16  6.3
	1979     2     allareas  age7   20      69.6   7.1
	1979     2     allareas  age8   8       70.5   6.6
	1979     2     allareas  age9   1       72     5.8
	1979     2     allareas  age10  0       0      5.6
	1979     3     allareas  age1   0       0      2.0
	1979     3     allareas  age2   0       0      3.7
	...
	1998     3     allareas  age1   0       0      2.0
	1998     3     allareas  age2   0       0      3.7
	1998     3     allareas  age3   17      45     6.2
	1998     3     allareas  age4   33      49.42  5.0
	1998     3     allareas  age5   78      56.26  5.5
	1998     3     allareas  age6   19      58.26  6.3
	1998     3     allareas  age7   16      63.31  7.1
	1998     3     allareas  age8   28      66.04  6.6
	1998     3     allareas  age9   3       74.67  5.8
	1998     3     allareas  age10  0       0      5.6

## Length Distribution Files

The 2 length distribution data files contain the number of samples in
each timestep/ area/age/length group combination for the 2 fleets. For
this likelihood component, there is no area aggregation, all the age
groups have been aggregated together into one age group, and the
length groups have been aggregated into 2cm length groups, as defined
by the aggregation files declared in the main likelihood file.

The likelihood function that is used to compare the data from these
files with the corresponding data from the model is a multinomial
function, given in equation 8 below: $\displaystyle \ell = 2 \sum_{\it
time}\sum_{\it areas}\sum_{\it age} \Bigg( \lo......it length} \Big(
N_{tral} \log {\frac{\pi_{tral}}{\sum \pi_{tral}}} \Big)\Bigg)$ (8)

where:
< π 	 is the model sample size for that time/area/age/length combination
< N 	 is the data sample size for that time/area/age/length combination

	;
	; Length distribution of the survey data for this haddock model
	;
	; year   step  area       age      length   number
	1985     2     allareas   allages  len1     0
	1985     2     allareas   allages  len2     0
	1985     2     allareas   allages  len3     4
	1985     2     allareas   allages  len4     112
	1985     2     allareas   allages  len5     1718
	1985     2     allareas   allages  len6     4419
	1985     2     allareas   allages  len7     3503
	1985     2     allareas   allages  len8     1408
	1985     2     allareas   allages  len9     403
	1985     2     allareas   allages  len10    216
	1985     2     allareas   allages  len11    880
	1985     2     allareas   allages  len12    2423
	1985     2     allareas   allages  len13    4311
	1985     2     allareas   allages  len14    4699
	1985     2     allareas   allages  len15    2663
	1985     2     allareas   allages  len16    1204
	1985     2     allareas   allages  len17    683
	1985     2     allareas   allages  len18    962
	1985     2     allareas   allages  len19    1588
	1985     2     allareas   allages  len20    1935
	1985     2     allareas   allages  len21    2508
	1985     2     allareas   allages  len22    2222
	...
	1999     2     allareas   allages  len34    192
	1999     2     allareas   allages  len35    87
	1999     2     allareas   allages  len36    137
	1999     2     allareas   allages  len37    63
	1999     2     allareas   allages  len38    39
	1999     2     allareas   allages  len39    11
	1999     2     allareas   allages  len40    16
	1999     2     allareas   allages  len41    1
	1999     2     allareas   allages  len42    1
	1999     2     allareas   allages  len43    1


	;
	; Length distribution of the commercial data for this haddock model
	;
	; year   step  area       age      length   number
	1979     1     allareas   allages  len1     0
	1979     1     allareas   allages  len2     0
	1979     1     allareas   allages  len3     0
	1979     1     allareas   allages  len4     0
	1979     1     allareas   allages  len5     0
	1979     1     allareas   allages  len6     0
	1979     1     allareas   allages  len7     0
	1979     1     allareas   allages  len8     0
	1979     1     allareas   allages  len9     0
	1979     1     allareas   allages  len10    0
	1979     1     allareas   allages  len11    0
	1979     1     allareas   allages  len12    0
	1979     1     allareas   allages  len13    0
	1979     1     allareas   allages  len14    0
	1979     1     allareas   allages  len15    1
	1979     1     allareas   allages  len16    3
	1979     1     allareas   allages  len17    11
	1979     1     allareas   allages  len18    17
	1979     1     allareas   allages  len19    32
	1979     1     allareas   allages  len20    53
	1979     1     allareas   allages  len21    97
	1979     1     allareas   allages  len22    145
	...
	1999     1     allareas   allages  len34    56
	1999     1     allareas   allages  len35    38
	1999     1     allareas   allages  len36    31
	1999     1     allareas   allages  len37    22
	1999     1     allareas   allages  len38    14
	1999     1     allareas   allages  len39    12
	1999     1     allareas   allages  len40    5
	1999     1     allareas   allages  len41    0
	1999     1     allareas   allages  len42    0
	1999     1     allareas   allages  len43    1

## Age-length Distribution Files

The 2 age-length distribution data files contain the number of samples
in each timestep/area/age/length group combination for the 2
fleets. For this likelihood component, there is no area or age group
aggregation, and the length groups have been aggregated into 2cm
length groups, as defined by the aggregation files declared in the
main likelihood file.

The only difference between the age-length distribution data files and
the length distribution data files is the age aggregation that takes
place for the length distribution. The likelihood function that is
used to compare the data from these files with the corresponding data
from the model is a multinomial function, given in equation 8 above.

	;
	; Age-length distribution of the survey data for this haddock model
	;
	; year   step  area       age    length   number
	1985     2     allareas   age1   len1     0
	1985     2     allareas   age2   len1     0
	1985     2     allareas   age3   len1     0
	1985     2     allareas   age4   len1     0
	1985     2     allareas   age5   len1     0
	1985     2     allareas   age6   len1     0
	1985     2     allareas   age7   len1     0
	1985     2     allareas   age8   len1     0
	1985     2     allareas   age9   len1     0
	1985     2     allareas   age10  len1     0
	1985     2     allareas   age1   len2     0
	1985     2     allareas   age2   len2     0
	1985     2     allareas   age3   len2     0
	1985     2     allareas   age4   len2     0
	1985     2     allareas   age5   len2     0
	1985     2     allareas   age6   len2     0
	1985     2     allareas   age7   len2     0
	1985     2     allareas   age8   len2     0
	1985     2     allareas   age9   len2     0
	1985     2     allareas   age10  len2     0
	1985     2     allareas   age1   len3     1
	1985     2     allareas   age2   len3     0
	...
	1999     2     allareas   age1   len43    0
	1999     2     allareas   age2   len43    0
	1999     2     allareas   age3   len43    0
	1999     2     allareas   age4   len43    0
	1999     2     allareas   age5   len43    0
	1999     2     allareas   age6   len43    0
	1999     2     allareas   age7   len43    0
	1999     2     allareas   age8   len43    0
	1999     2     allareas   age9   len43    0
	1999     2     allareas   age10  len43    0


	;
	; Age-length distribution of the commercial data for this haddock model
	;
	; year   step  area       age    length   number
	1979     1     allareas   age1   len1     0
	1979     1     allareas   age2   len1     0
	1979     1     allareas   age3   len1     0
	1979     1     allareas   age4   len1     0
	1979     1     allareas   age5   len1     0
	1979     1     allareas   age6   len1     0
	1979     1     allareas   age7   len1     0
	1979     1     allareas   age8   len1     0
	1979     1     allareas   age9   len1     0
	1979     1     allareas   age10  len1     0
	1979     1     allareas   age1   len2     0
	1979     1     allareas   age2   len2     0
	1979     1     allareas   age3   len2     0
	1979     1     allareas   age4   len2     0
	1979     1     allareas   age5   len2     0
	1979     1     allareas   age6   len2     0
	1979     1     allareas   age7   len2     0
	1979     1     allareas   age8   len2     0
	1979     1     allareas   age9   len2     0
	1979     1     allareas   age10  len2     0
	1979     1     allareas   age1   len3     0
	1979     1     allareas   age2   len3     0
	...
	1998     3     allareas   age1   len43    0
	1998     3     allareas   age2   len43    0
	1998     3     allareas   age3   len43    0
	1998     3     allareas   age4   len43    0
	1998     3     allareas   age5   len43    0
	1998     3     allareas   age6   len43    0
	1998     3     allareas   age7   len43    0
	1998     3     allareas   age8   len43    0
	1998     3     allareas   age9   len43    0
	1998     3     allareas   age10  len43    0

## Survey Index File

The survey index data file contains the number of samples in each
timestep/area/length group combination for the 6 survey indices
defined in the main likelihood file. For this likelihood component,
there is no area or age group aggregation, and the length groups have
been aggregated into 5cm length groups, as defined by the various
length aggregation files declared in the main likelihood file.

The likelihood function that is used to compare the data from these
files with the corresponding data from the model is a log linear
regression function. For the regression line (specified in the main
likelihood file), the slope is fixed and the intercept calculated by
Gadget. This is given in equation 9 below: $\displaystyle \ell =
\sum_{\it time}\Big(I_{t} - (\alpha + \beta N_{t})\Big)^2$ (9)

where:
< I 	 is the survey index
< N 	 is the corresponding index calculated in the Gadget model
< α 	 is the intercept of the regression line
< β 	 is the slope of the regression line (which has been set to 1)

	;
	; Survey index data for this haddock model
	;
	; year  step  area      length  number
	1985    1     allareas  mlen10  258
	1986    1     allareas  mlen10  808
	1987    1     allareas  mlen10  286
	1988    1     allareas  mlen10  131
	1989    1     allareas  mlen10  361
	1990    1     allareas  mlen10  568
	1991    1     allareas  mlen10  1163
	1992    1     allareas  mlen10  1713
	1993    1     allareas  mlen10  642
	1994    1     allareas  mlen10  782
	1995    1     allareas  mlen10  171
	1996    1     allareas  mlen10  508
	1997    1     allareas  mlen10  217
	1998    1     allareas  mlen10  197
	1999    1     allareas  mlen10  2690
	1985    1     allareas  mlen15  20063
	1986    1     allareas  mlen15  91563
	1987    1     allareas  mlen15  20086
	1988    1     allareas  mlen15  12051
	1989    1     allareas  mlen15  8853
	1990    1     allareas  mlen15  64132
	1991    1     allareas  mlen15  76318
	...
	1990    1     allareas  mlen75  1035
	1991    1     allareas  mlen75  945
	1992    1     allareas  mlen75  2101
	1993    1     allareas  mlen75  948
	1994    1     allareas  mlen75  533
	1995    1     allareas  mlen75  371
	1996    1     allareas  mlen75  553
	1997    1     allareas  mlen75  318
	1998    1     allareas  mlen75  505
	1999    1     allareas  mlen75  674


## Print File

The printfile defines the content of the output files that will be
generated when a stochastic run of Gadget is performed (by specifying
the ''-s'' command line option when Gadget is started). This output is
defined by specifying details of the stock, area, age and length
groups and the name of the output file that is to be generated.

In this example there are 4 output files to be generated. The first
file (created by ''StockStdPrinter'') contains an age-based summary of
the stock, giving details of the number, length, mean weight and
consumption for each timestep/area/age group combination. The next two
files (created by ''StockPrinter'' and ''StockFullPrinter'') give
details of the number and mean weight for each timestep/area/age
group/length group combination - the difference being the amount of
aggregation that takes place. The 4th file (created by
''PredatorPrinter'') contains information about the predator/prey
combination (with the predator being the fleets) and gives details of
the biomass consumed for each timestep/area/predator length group/prey
length group combination.

The final two files deal with the output from the likelihood
components that have been used to compare this modelled population to
observed data. The first file (created by ''LikelihoodPrinter'') gives
detailed information on the modelled data compared to the data
specified for the 'ldist.sur' likelihood component, as defined in the
likelihood file. The second file (created by
''LikelihoodSummaryPrinter'') gives a summary of the likelihood
information from each timestep in the model.

	;
	; Print file for this haddock model
	;
	[component]
	type            stockstdprinter
	stockname       had              ; name of the stock
	printfile       out/had.std      ; name for the output file
	yearsandsteps   all all          ; timesteps to print
	;
	[component]
	type            stockprinter
	stocknames      had              ; name of the stock (could be more than one)
	areaaggfile     allarea.agg      ; area aggregation file
	ageaggfile      allage.agg       ; age aggregation file
	lenaggfile      len.agg          ; length aggregation file
	printfile       out/had.print    ; name for the output file
	yearsandsteps   all all          ; timesteps to print
	;
	[component]
	type            stockfullprinter
	stockname       had              ; name of the stock
	printfile       out/had.stock    ; name for the output file
	yearsandsteps   all all          ; timesteps to print
	;
	[component]
	type            predatorprinter
	predators       comm future      ; name of the predators (fleets)
	preys           had              ; name of the preys
	areaaggfile     allarea.agg      ; area aggregation file
	predlenaggfile  alllen.agg       ; length aggregation file for the predators
	preylenaggfile  len.agg          ; length aggregation file for the preys
	printfile       out/had.fleet    ; name for the output file
	yearsandsteps   all all          ; timesteps to print
	;
	[component]
	type            likelihoodprinter
	likelihood      ldist.sur        ; name of the likelihood component
	printfile       out/ldist.sur    ; name for the output file
	;
	[component]
	type            likelihoodsummaryprinter
	printfile       out/summary.txt  ; name for the output file

### StockStdPrinter Output

The output file that is generated by the stockstdprinter printer class
is given below. This class summarises the data available for the
stock, giving the number, mean length, mean weight, standard deviation
of the length, number consumed and biomass consumed for each
timestep/area/age group combination.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Standard output file for the stock had
	; Printing the following information at the end of each timestep
	; year-step-area-age-number-mean length-mean weight-stddev length- ...
	1978    1    1    1 2.2377895e+08      16.41  0.0444454      2.25  ...
	1978    1    1    2 1.0285316e+08    29.8986   0.239963   3.61989  ...
	1978    1    1    3     13576599    39.4734   0.585367    4.57133  ...
	1978    1    1    4     20759863    46.0429   0.940242    5.24777  ...
	1978    1    1    5     46760270     51.507    1.31759     5.7698  ...
	1978    1    1    6    1808503.8    55.7022    1.65875     6.0014  ...
	1978    1    1    7    741868.46    60.4509      2.101    6.18222  ...
	1978    1    1    8     11378896    67.6647    2.94619    7.95431  ...
	1978    1    1    9     25158880    62.5636    2.38451    8.94957  ...
	1978    1    1   10    771144.33    64.6152    2.61065     8.9165  ...
	1978    2    1    1 1.9745062e+08    20.9102  0.0699865   3.39601  ...
	1978    2    1    2     93984352    32.4556   0.309357    4.01812  ...
	1978    2    1    3     12697802    41.6404   0.693019    5.02845  ...
	1978    2    1    4     18951490    48.1062    1.07242    5.57866  ...
	1978    2    1    5     41693765      53.43    1.46936    6.00304  ...
	1978    2    1    6    1588083.2    57.5301    1.82684     6.1989  ...
	1978    2    1    7    643580.75    62.1815    2.28764    6.34419  ...
	1978    2    1    8    9554725.5    69.1975    3.15214    7.96035  ...
	1978    2    1    9     20769197    64.1639    2.56871    8.96043  ...
	1978    2    1   10    588659.57    66.1818    2.80272    8.91073  ...
	1978    3    1    1 1.7420549e+08    22.8453   0.104041   3.72832  ...
	1978    3    1    2     85845884    34.6377   0.387975    4.60743  ...
	...
	2006    4    1    1     61764706     25.812   0.147929    4.15511  ...
	2006    4    1    2     42757071    35.5567    0.41957     5.6528  ...
	2006    4    1    3     31974330    44.0339   0.826287     6.5416  ...
	2006    4    1    4     20854393    51.4965    1.33828    7.15737  ...
	2006    4    1    5     11854129    58.3533    1.95177    7.69252  ...
	2006    4    1    6    6130993.1    64.9045    2.67258    8.10018  ...
	2006    4    1    7    2971731.6    71.2803    3.50628    8.20964  ...
	2006    4    1    8    3630883.7    76.8385    4.38653    7.65143  ...
	2006    4    1    9    487102.34    80.8852    5.20972    6.94081  ...
	2006    4    1   10    157279.59    86.1688     6.7596    5.23485  ...

### StockPrinter Output

The output file that is generated by the stockprinter printer class is
given below. This class gives a more detailed view of the information
available for the stock, giving the number and mean weight for each
timestep/area/age group/length group combination specified in the
aggregation files. The labels displayed for the area, age group and
length group come from those given in the aggregation files.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Output file for the following stocks had
	; Printing the following information at the end of each timestep
	; year-step-area-age-length-number-weight
	1978    1   allareas    allages       len1    995.19332 0.0019135364
	1978    1   allareas    allages       len2    43025.981 0.0045467325
	1978    1   allareas    allages       len3    860843.64 0.0088680894
	1978    1   allareas    allages       len4    8016028.2  0.015264019
	1978    1   allareas    allages       len5     34940181  0.024108638
	1978    1   allareas    allages       len6     71630096  0.035783594
	1978    1   allareas    allages       len7     69270462  0.050703387
	1978    1   allareas    allages       len8     31771142  0.069301761
	1978    1   allareas    allages       len9    7945568.1  0.091583901
	1978    1   allareas    allages      len10    5266105.1   0.11581048
	1978    1   allareas    allages      len11     11629097   0.14588475
	1978    1   allareas    allages      len12     19660149     0.181742
	1978    1   allareas    allages      len13     23220662   0.22333713
	1978    1   allareas    allages      len14     20105226   0.27015473
	1978    1   allareas    allages      len15     13805359   0.32280877
	1978    1   allareas    allages      len16    8682302.8    0.3848269
	1978    1   allareas    allages      len17    6180120.7   0.46348323
	1978    1   allareas    allages      len18    5683633.2   0.55737393
	1978    1   allareas    allages      len19    6244621.3   0.65737678
	1978    1   allareas    allages      len20      7248994   0.76533392
	1978    1   allareas    allages      len21    8397645.4   0.87947095
	1978    1   allareas    allages      len22    9384257.1    1.0001353
	...
	2006    4   allareas    allages      len34    1389557.6    3.3507992
	2006    4   allareas    allages      len35    1212053.6    3.6419917
	2006    4   allareas    allages      len36    1042789.9    3.9465744
	2006    4   allareas    allages      len37    887725.93    4.2648003
	2006    4   allareas    allages      len38    742220.95    4.5968777
	2006    4   allareas    allages      len39    604583.65      4.94306
	2006    4   allareas    allages      len40    476775.63    5.3037262
	2006    4   allareas    allages      len41    362510.56    5.6793596
	2006    4   allareas    allages      len42       263106    6.0667065
	2006    4   allareas    allages      len43    528590.59    7.2129296

### StockFullPrinter Output

The output file that is generated by the stockfullprinter printer
class is given below. This class gives a more detailed view of the
information available for the stock, giving the number and mean weight
for each timestep/area/age group/length group combination with no
aggregation.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Full output file for the stock had
	; Printing the following information at the end of each timestep
	; year-step-area-age-length-number-mean weight
	1978    1    1    1    5    103.35986 0.0011529668
	1978    1    1    1    6    891.83256  0.002001684
	1978    1    1    1    7    6315.7972 0.0031912175
	1978    1    1    1    8    36710.176 0.0047799422
	1978    1    1    1    9    175129.05 0.0068264487
	1978    1    1    1   10    685714.52 0.0093895174
	1978    1    1    1   11    2203646.6  0.012528097
	1978    1    1    1   12    5812379.6   0.01630129
	1978    1    1    1   13     12582862  0.020768335
	1978    1    1    1   14     22357248  0.025988599
	1978    1    1    1   15     32603985  0.032021568
	1978    1    1    1   16     39024420  0.038926836
	1978    1    1    1   17     38336783  0.046764099
	1978    1    1    1   18     30910662  0.055593151
	1978    1    1    1   19     20455702  0.065473874
	1978    1    1    1   20     11110508  0.076466239
	1978    1    1    1   21    4952983.2  0.088630294
	1978    1    1    1   22    1812229.7   0.10202617
	1978    1    1    1   23    544218.21   0.11671406
	1978    1    1    1   24     134136.3   0.13275425
	1978    1    1    1   25      27135.2   0.15020706
	1978    1    1    1   26    4505.3979   0.16913291
	...
	2006    4    1   10   81    4698.1858    5.1098441
	2006    4    1   10   82    5209.5173    5.2807009
	2006    4    1   10   83    5699.0684    5.4551787
	2006    4    1   10   84    6151.7583    5.6333312
	2006    4    1   10   85    6552.6155    5.8152331
	2006    4    1   10   86    6889.0637    6.0010163
	2006    4    1   10   87    7149.2775    6.1908312
	2006    4    1   10   88    7113.2096    6.3746684
	2006    4    1   10   89    6770.1158    6.5459936
	2006    4    1   10   90    76601.376    8.1745838

### PredatorPrinter Output

The output file that is generated by the predatorprinter printer class
is given below. This class gives a detailed view of the information
available for the predator/prey combination specified in the
printfile, giving the biomass consumed for each timestep/area/predator
length group/prey length group combination specified in the
aggregation files. The labels displayed for the area, predator length
group and prey length group come from those given in the aggregation
files. Note that there is only one predator length group in this
example, since the predator is a combination of the commercial fleet
and the future fleet.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Predation output file for the following predators comm future
	; Consuming the following preys had
	; Printing the following information at the end of each timestep
	; year-step-area-pred length-prey length-biomass consumed
	1978    1   allareas        all       len1 1.9095251e-10
	1978    1   allareas        all       len2 2.769502e-09
	1978    1   allareas        all       len3 7.9066149e-08
	1978    1   allareas        all       len4 6.4153963e-06
	1978    1   allareas        all       len5 0.00044759835
	1978    1   allareas        all       len6  0.018265668
	1978    1   allareas        all       len7   0.48086564
	1978    1   allareas        all       len8    7.7410596
	1978    1   allareas        all       len9    79.910456
	1978    1   allareas        all      len10    454.94757
	1978    1   allareas        all      len11    1645.1663
	1978    1   allareas        all      len12    3701.4821
	1978    1   allareas        all      len13     5278.777
	1978    1   allareas        all      len14    5011.2433
	1978    1   allareas        all      len15    4426.3962
	1978    1   allareas        all      len16    6467.4621
	1978    1   allareas        all      len17    13664.742
	1978    1   allareas        all      len18    28604.035
	1978    1   allareas        all      len19    56645.296
	1978    1   allareas        all      len20    107851.13
	1978    1   allareas        all      len21    186017.84
	...
	2006    4   allareas        all      len34    606481.45
	2006    4   allareas        all      len35       580115
	2006    4   allareas        all      len36    542142.07
	2006    4   allareas        all      len37    496458.59
	2006    4   allareas        all      len38    442529.33
	2006    4   allareas        all      len39    381503.37
	2006    4   allareas        all      len40    316747.06
	2006    4   allareas        all      len41    252781.86
	2006    4   allareas        all      len42    192291.71
	2006    4   allareas        all      len43    447616.23

### LikelihoodPrinter Output

The output file that is generated by the likelihoodprinter printer
class is given below. This class gives a detailed view of the internal
model information used when calculating the likelihood score for the
likelihood component, in the same format as the data in the input
file. The likelihood component that has been used for this example
print file is ''ldist.sur'', which is a 'CatchDistribution' likelihood
component, so the output here is in the same format as the Length
Distribution Files.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Likelihood output file for the likelihood component ldist.sur
	; year-step-area-age-length-number
	1985    2   allareas    allages       len1 0.0034991834
	1985    2   allareas    allages       len2   0.23145168
	1985    2   allareas    allages       len3    6.9983078
	1985    2   allareas    allages       len4    96.714259
	1985    2   allareas    allages       len5     609.8366
	1985    2   allareas    allages       len6     1750.113
	1985    2   allareas    allages       len7    2285.2735
	1985    2   allareas    allages       len8    1421.8431
	1985    2   allareas    allages       len9    720.93139
	1985    2   allareas    allages      len10    897.46163
	1985    2   allareas    allages      len11    1394.5465
	1985    2   allareas    allages      len12    1756.9466
	1985    2   allareas    allages      len13    1781.9329
	1985    2   allareas    allages      len14    1521.8945
	1985    2   allareas    allages      len15      1198.61
	1985    2   allareas    allages      len16    954.11375
	1985    2   allareas    allages      len17    825.81861
	1985    2   allareas    allages      len18    781.30324
	1985    2   allareas    allages      len19    787.07736
	1985    2   allareas    allages      len20    833.37398
	1985    2   allareas    allages      len21    883.49435
	1985    2   allareas    allages      len22    924.91969
	1985    2   allareas    allages      len23    949.26513
	...
	1999    2   allareas    allages      len34    195.33312
	1999    2   allareas    allages      len35     149.5913
	1999    2   allareas    allages      len36    112.17573
	1999    2   allareas    allages      len37    83.272268
	1999    2   allareas    allages      len38    61.178262
	1999    2   allareas    allages      len39    44.320533
	1999    2   allareas    allages      len40    31.505514
	1999    2   allareas    allages      len41    21.860349
	1999    2   allareas    allages      len42    14.646656
	1999    2   allareas    allages      len43    23.943981

### LikelihoodSummaryPrinter Output

The output file that is generated by the likelihoodsummaryprinter
printer class is given below. This class gives a summary view of the
scores on each timestep from each of the likelihood components that
have been specified for the current model.

	; Gadget version 2.1.05 running on hafnasandur Fri Dec 15 11:47:45 2006
	; Summary likelihood information from the current run
	; year-step-area-component-weight-likelihood value
	1989    2   allareas    meanl.sur    2e-06    225.29312
	1990    2   allareas    meanl.sur    2e-06     305.8889
	1991    2   allareas    meanl.sur    2e-06    296.40938
	1992    2   allareas    meanl.sur    2e-06     384.3281
	1993    2   allareas    meanl.sur    2e-06    460.41256
	1994    2   allareas    meanl.sur    2e-06    963.15011
	1995    2   allareas    meanl.sur    2e-06     382.0248
	1996    2   allareas    meanl.sur    2e-06     599.1694
	1997    2   allareas    meanl.sur    2e-06    513.14933
	1998    2   allareas    meanl.sur    2e-06    531.96395
	1999    2   allareas    meanl.sur    2e-06    573.03989
	1979    1   allareas  meanl.catch    3e-07    160.85429
	1979    2   allareas  meanl.catch    3e-07    98.793133
	1979    3   allareas  meanl.catch    3e-07    23.869148
	1979    4   allareas  meanl.catch    3e-07    202.96922
	1980    1   allareas  meanl.catch    3e-07    236.41646
	1980    2   allareas  meanl.catch    3e-07     44.15178
	1980    4   allareas  meanl.catch    3e-07    29.887795
	1981    1   allareas  meanl.catch    3e-07    97.947644
	1981    2   allareas  meanl.catch    3e-07    43.617345
	1981    3   allareas  meanl.catch    3e-07    140.49857
	1981    4   allareas  meanl.catch    3e-07    13.503321
	1982    1   allareas  meanl.catch    3e-07    87.779096
	...
	1997    4   allareas alkeys.catch    2e-06    902.57365
	1998    1   allareas alkeys.catch    2e-06    617.79915
	1998    2   allareas alkeys.catch    2e-06    572.60255
	1998    3   allareas alkeys.catch    2e-06    271.22842
	all   all   allareas         si10    0.007    4.5148467
	all   all   allareas         si15     0.01    2.6221572
	all   all   allareas         si20     0.01    5.2799002
	all   all   allareas     si25to45     0.01    11.486399
	all   all   allareas     si50to60     0.01    4.8039545
	all   all   allareas     si65to75    0.007    6.9859794


### Parameter File

The parameter file is used to specify the initial values for the
switches that are to be used in the Gadget model. This file is
specified by a "-i <filename	" command line option when Gadget is
started, and contains a list of all the switches, their initial value,
the lower and upper bounds and a flag to note whether the optimiser
should optimise that switch or not.

	;
	; Reference parameter file for this haddock model
	;
	switch   value        lower   upper  optimize
	grq0     6.3421717    1       10     1 ; q0 in growth function
	grq2     17.020301    5       20     1 ; q2, q3 in growth function
	bbeta    11.792983    0.1     5000   1 ; beta in beta-binomial
	inage2   0.011234341  0.00001 1      1 ; initial number of age 2 fish
	inage3   0.0023677952 0.00001 1      1 ; initial number of age 3 fish
	inage4   0.0060429722 0.00001 1      1 ; initial number of age 4 fish
	inage5   0.022761935  0.00001 1      1 ; initial number of age 5 fish
	inage6   0.0014660210 0.00001 1      1 ; initial number of age 6 fish
	inage7   0.0010000000 0.00001 1      1 ; initial number of age 7 fish
	inage8   0.026070485  0.00001 1      1 ; initial number of age 8 fish
	inage9   0.096904951  0.00001 1      1 ; initial number of age 9 fish
	rec78    22.377895    0.2     34     1 ; number of recruits in 1978
	rec79    9.9845325    0.2     34     1 ; number of recruits in 1979
	rec80    1.2187327    0.2     34     1 ; number of recruits in 1980
	rec81    11.258073    0.2     34     1 ; number of recruits in 1981
	rec82    4.8916410    0.2     34     1 ; number of recruits in 1982
	rec83    3.5328430    0.2     34     1 ; number of recruits in 1983
	rec84    8.2212940    0.2     34     1 ; number of recruits in 1984
	rec85    9.0904755    0.2     34     1 ; number of recruits in 1985
	rec86    24.934956    0.2     34     1 ; number of recruits in 1986
	rec87    9.0852408    0.2     34     1 ; number of recruits in 1987
	rec88    5.0242726    0.2     34     1 ; number of recruits in 1988
	...
	rec95    6.5406113    0.2     34     1 ; number of recruits in 1995
	rec96    14.777468    0.2     34     1 ; number of recruits in 1996
	rec97    4.2329148    0.2     34     1 ; number of recruits in 1997
	rec98    7.9890151    0.2     34     1 ; number of recruits in 1998
	rec99    24.796618    0.2     34     1 ; number of recruits in 1999
	acomm    -10.342371   -100    -1     1 ; alpha in fleet suitability
	bcomm    0.21405130   0.1     10     1 ; beta in fleet suitability
	asur     -4.3391371   -100    -1     1 ; alpha in survey suitability
	bsur     0.22464659   0.1     10     1 ; beta in survey suitability
	mult     0.5          0.1     1      1 ; multiplier for future fleet

