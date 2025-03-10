#include "PX_Wavelet.h"


static px_void PX_DWTReverseVector(px_double f[], px_int size, px_int end_size)
{
    px_int i;
    if (size <= end_size)
    {
        return;
    }

    for (i = 0; i < size / 2; i++)
    {
        px_double tmp = f[i];
        f[i] = f[size / 2 + i];
        f[size / 2 + i] = tmp;
    }
    PX_DWTReverseVector(f, size / 2, end_size);
}

static px_void PX_DWT_g0_2_g1(px_double g0[], px_double g1[], px_int size)
{
    px_int n;
    for (n = 0; n < size; n++)
    {
        g1[n] = PX_pow_ii(-1, n) * g0[size - n - 1];
    }
}

static px_void PX_DWT_g0_2_h0(px_double g0[], px_double h0[], px_int size)
{
    px_int n;
    for (n = 0; n < size; n++)
    {
        h0[n] = g0[size - n - 1];
    }
}

static px_void PX_DWT_g1_2_h1(px_double g1[], px_double h1[], px_int size)
{
    px_int n;
    for (n = 0; n < size; n++)
    {
        h1[n] = g1[size - n - 1];
    }
}

px_bool PX_DWTSetKernel(px_memorypool *mp, PX_DWT *dwt, px_double g0[], px_int size)
{
    px_double g1[PX_DWT_FILTER_MAX_ORDER];
    px_double h0[PX_DWT_FILTER_MAX_ORDER];
    px_double h1[PX_DWT_FILTER_MAX_ORDER];

    PX_DWT_g0_2_g1(g0, g1, size);
    PX_DWT_g0_2_h0(g0, h0, size);
    PX_DWT_g1_2_h1(g1, h1, size);

    if (!PX_LTIInitialize(&dwt->h0, mp, size, size)) { return PX_FALSE; }
    if (!PX_LTIInitialize(&dwt->h1, mp, size, size)) { return PX_FALSE; }
    if (!PX_LTIInitialize(&dwt->g0, mp, size, size)) { return PX_FALSE; }
    if (!PX_LTIInitialize(&dwt->g1, mp, size, size)) { return PX_FALSE; }

    PX_LTISetKernel(&dwt->h1, 0, h1);
    PX_LTISetKernel(&dwt->h0, 0, h0);
    PX_LTISetKernel(&dwt->g1, 0, g1);
    PX_LTISetKernel(&dwt->g0, 0, g0);
    return PX_TRUE;
}

px_bool PX_DWTInitialize(px_memorypool *mp, PX_DWT *dwt, px_char name[8])
{
    dwt->mp = mp;
    if (PX_strequ2(name, "haar"))
    {
        px_double g0[] =
        {
            0.7071067811865475244008443621048490392848359376884740365883398,
            0.7071067811865475244008443621048490392848359376884740365883398,
        };
        dwt->type = PX_DWT_BASE_TYPE_HAAR;
        PX_strset(dwt->name, "haar");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db2"))
    {
        px_double g0[] =
        {
            0.4829629131445341433748715998644486838169524195042022752011715,
            0.8365163037378079055752937809168732034593703883484392934953414,
            0.2241438680420133810259727622404003554678835181842717613871683,
            -0.1294095225512603811744494188120241641745344506599652569070016
        };
        dwt->type = PX_DWT_BASE_TYPE_DB2;
        PX_strset(dwt->name, "db2");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db3"))
    {
        px_double g0[] =
        {
            0.3326705529500826159985115891390056300129233992450683597084705,
            0.8068915093110925764944936040887134905192973949948236181650920,
            0.4598775021184915700951519421476167208081101774314923066433867,
            -0.1350110200102545886963899066993744805622198452237811919756862,
            -0.08544127388202666169281916918177331153619763898808662976351748,
            0.03522629188570953660274066471551002932775838791743161039893406
        };
        dwt->type = PX_DWT_BASE_TYPE_DB3;
        PX_strset(dwt->name, "db3");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db4"))
    {
        px_double g0[] =
        {
            0.2303778133088965008632911830440708500016152482483092977910968,
            0.7148465705529156470899219552739926037076084010993081758450110,
            0.6308807679298589078817163383006152202032229226771951174057473,
            -0.02798376941685985421141374718007538541198732022449175284003358,
            -0.1870348117190930840795706727890814195845441743745800912057770,
            0.03084138183556076362721936253495905017031482172003403341821219,
            0.03288301166688519973540751354924438866454194113754971259727278,
            -0.01059740178506903210488320852402722918109996490637641983484974
        };
        dwt->type = PX_DWT_BASE_TYPE_DB4;
        PX_strset(dwt->name, "db4");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db5"))
    {
        px_double g0[] =
        {
            1.601023979741929144807237480204207336505441246250578327725699e-01,
            6.038292697971896705401193065250621075074221631016986987969283e-01,
            7.243085284377729277280712441022186407687562182320073725767335e-01,
            1.384281459013207315053971463390246973141057911739561022694652e-01,
            -2.422948870663820318625713794746163619914908080626185983913726e-01,
            -3.224486958463837464847975506213492831356498416379847225434268e-02,
            7.757149384004571352313048938860181980623099452012527983210146e-02,
            -6.241490212798274274190519112920192970763557165687607323417435e-03,
            -1.258075199908199946850973993177579294920459162609785020169232e-02,
            3.335725285473771277998183415817355747636524742305315099706428e-03
        };
        dwt->type = PX_DWT_BASE_TYPE_DB5;
        PX_strset(dwt->name, "db5");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db6"))
    {
        px_double g0[] =
        {
            1.115407433501094636213239172409234390425395919844216759082360e-01,
            4.946238903984530856772041768778555886377863828962743623531834e-01,
            7.511339080210953506789344984397316855802547833382612009730420e-01,
            3.152503517091976290859896548109263966495199235172945244404163e-01,
            -2.262646939654398200763145006609034656705401539728969940143487e-01,
            -1.297668675672619355622896058765854608452337492235814701599310e-01,
            9.750160558732304910234355253812534233983074749525514279893193e-02,
            2.752286553030572862554083950419321365738758783043454321494202e-02,
            -3.158203931748602956507908069984866905747953237314842337511464e-02,
            5.538422011614961392519183980465012206110262773864964295476524e-04,
            4.777257510945510639635975246820707050230501216581434297593254e-03,
            -1.077301085308479564852621609587200035235233609334419689818580e-03
        };
        dwt->type = PX_DWT_BASE_TYPE_DB6;
        PX_strset(dwt->name, "db6");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db7"))
    {
        px_double g0[] =
        {
            7.785205408500917901996352195789374837918305292795568438702937e-02,
            3.965393194819173065390003909368428563587151149333287401110499e-01,
            7.291320908462351199169430703392820517179660611901363782697715e-01,
            4.697822874051931224715911609744517386817913056787359532392529e-01,
            -1.439060039285649754050683622130460017952735705499084834401753e-01,
            -2.240361849938749826381404202332509644757830896773246552665095e-01,
            7.130921926683026475087657050112904822711327451412314659575113e-02,
            8.061260915108307191292248035938190585823820965629489058139218e-02,
            -3.802993693501441357959206160185803585446196938467869898283122e-02,
            -1.657454163066688065410767489170265479204504394820713705239272e-02,
            1.255099855609984061298988603418777957289474046048710038411818e-02,
            4.295779729213665211321291228197322228235350396942409742946366e-04,
            -1.801640704047490915268262912739550962585651469641090625323864e-03,
            3.537137999745202484462958363064254310959060059520040012524275e-04
        };
        dwt->type = PX_DWT_BASE_TYPE_DB7;
        PX_strset(dwt->name, "db7");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "db8"))
    {
        px_double g0[] =
        {
            5.441584224310400995500940520299935503599554294733050397729280e-02,
            3.128715909142999706591623755057177219497319740370229185698712e-01,
            6.756307362972898068078007670471831499869115906336364227766759e-01,
            5.853546836542067127712655200450981944303266678053369055707175e-01,
            -1.582910525634930566738054787646630415774471154502826559735335e-02,
            -2.840155429615469265162031323741647324684350124871451793599204e-01,
            4.724845739132827703605900098258949861948011288770074644084096e-04,
            1.287474266204784588570292875097083843022601575556488795577000e-01,
            -1.736930100180754616961614886809598311413086529488394316977315e-02,
            -4.408825393079475150676372323896350189751839190110996472750391e-02,
            1.398102791739828164872293057263345144239559532934347169146368e-02,
            8.746094047405776716382743246475640180402147081140676742686747e-03,
            -4.870352993451574310422181557109824016634978512157003764736208e-03,
            -3.917403733769470462980803573237762675229350073890493724492694e-04,
            6.754494064505693663695475738792991218489630013558432103617077e-04,
            -1.174767841247695337306282316988909444086693950311503927620013e-04
        };
        dwt->type = PX_DWT_BASE_TYPE_DB8;
        PX_strset(dwt->name, "db8");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym2"))
    {
        px_double g0[] =
        {
            0.48296291314469025, 0.83651630373746899,
            0.22414386804185735, -0.12940952255092145
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM2;
        PX_strset(dwt->name, "sym2");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym3"))
    {
        px_double g0[] =
        {
            0.33267055295095688, 0.80689150931333875,
            0.45987750211933132, -0.13501102001039084,
            -0.085441273882241486, 0.035226291882100656
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM3;
        PX_strset(dwt->name, "sym3");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym4"))
    {
        px_double g0[] =
        {
            0.032223100604042702, -0.012603967262037833,
            -0.099219543576847216, 0.29785779560527736,
            0.80373875180591614, 0.49761866763201545,
            -0.02963552764599851, -0.075765714789273325
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM4;
        PX_strset(dwt->name, "sym4");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym5"))
    {
        px_double g0[] =
        {
            0.019538882735286728, -0.021101834024758855,
            -0.17532808990845047, 0.016602105764522319,
            0.63397896345821192, 0.72340769040242059,
            0.1993975339773936, -0.039134249302383094,
            0.029519490925774643, 0.027333068345077982
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM5;
        PX_strset(dwt->name, "sym5");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym6"))
    {
        px_double g0[] =
        {
            -0.007800708325034148, 0.0017677118642428036,
            0.044724901770665779, -0.021060292512300564,
            -0.072637522786462516, 0.3379294217276218,
            0.787641141030194, 0.49105594192674662,
            -0.048311742585632998, -0.11799011114819057,
            0.0034907120842174702, 0.015404109327027373
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM6;
        PX_strset(dwt->name, "sym6");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }

    if (PX_strequ2(name, "sym7"))
    {
        px_double g0[] =
        {
            0.010268176708511255, 0.0040102448715336634,
            -0.10780823770381774, -0.14004724044296152,
            0.28862963175151463, 0.76776431700316405,
            0.5361019170917628, 0.017441255086855827,
            -0.049552834937127255, 0.067892693501372697,
            0.03051551316596357, -0.01263630340325193,
            -0.0010473848886829163, 0.0026818145682578781
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM7;
        PX_strset(dwt->name, "sym7");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }


    if (PX_strequ2(name, "sym8"))
    {
        px_double g0[] =
        {
            0.0018899503327594609, -0.0003029205147213668,
            -0.014952258337048231, 0.0038087520138906151,
            0.049137179673607506, -0.027219029917056003,
            -0.051945838107709037, 0.3644418948353314,
            0.77718575170052351, 0.48135965125837221,
            -0.061273359067658524, -0.14329423835080971,
            0.0076074873249176054, 0.031695087811492981,
            -0.00054213233179114812, -0.0033824159510061256
        };
        dwt->type = PX_DWT_BASE_TYPE_SYM8;
        PX_strset(dwt->name, "sym8");
        return PX_DWTSetKernel(mp, dwt, g0, PX_COUNTOF(g0));
    }




    return PX_FALSE;
}

px_bool PX_DWTGoEx(PX_DWT *dwt, px_double in[], px_int in_size, px_double out[])
{
    px_int i, j, n;
    px_double *hi_pool, *lo_pool;
    px_int fliterlen = dwt->h1.b_count;

    if (in_size < fliterlen)
    {
        return PX_TRUE;
    }

    if ((in_size & (fliterlen - 1)) || in_size <= 0)
    {
        return PX_FALSE;
    }
    hi_pool = (px_double *)MP_Malloc(dwt->mp, (in_size + fliterlen - 1) * sizeof(px_double));
    lo_pool = (px_double *)MP_Malloc(dwt->mp, (in_size + fliterlen - 1) * sizeof(px_double));

    if (!hi_pool || !lo_pool)
    {
        return PX_FALSE;
    }

    //high pass
    for (i = 0; i < in_size; i++)
    {
        hi_pool[i] = PX_LTIGo(&dwt->h1, in[i]);
    }
    for (j = 0; j < fliterlen - 1; j++, i++)
    {
        hi_pool[i] = PX_LTIGo(&dwt->h1, 0);
    }

    PX_LTIReset(&dwt->h1);

    //low pass
    for (i = 0; i < in_size ; i++)
    {
        lo_pool[i] = PX_LTIGo(&dwt->h0, in[i]);
    }
    for (j = 0; j < fliterlen - 1; j++, i++)
    {
        lo_pool[i] = PX_LTIGo(&dwt->h0, 0);
    }

    PX_LTIReset(&dwt->h0);

    n = in_size / 2;

    for (i = 0; i < n; i ++)
    {
        out[i] = hi_pool[i * 2 + fliterlen - 1];
    }

    for (i = 0; i < n; i++)
    {
        out[i + n] = lo_pool[i * 2 + fliterlen - 1];
    }


    MP_Free(dwt->mp, hi_pool);
    MP_Free(dwt->mp, lo_pool);

    PX_DWTGoEx(dwt, out + in_size / 2, in_size / 2, out + in_size / 2);
    return PX_TRUE;
}
px_bool PX_DWTGo(PX_DWT *dwt, px_double in[], px_int in_size, px_double out[])
{
    if (in_size < dwt->g1.b_count)
    {
        return PX_FALSE;
    }
    if (!PX_DWTGoEx(dwt, in, in_size, out)) { return PX_FALSE; }
    PX_DWTReverseVector(out, in_size, dwt->g1.b_count / 2);
    return PX_TRUE;
}
px_bool PX_IDWTGoEx(PX_DWT *dwt, px_double in[], px_int in_size, px_double out[],
                    px_int restore_size)
{
    px_double *hi_pool, * lo_pool;
    px_int fliterlen = dwt->h1.b_count;
    px_int i, j;
    if (in_size == restore_size)
    {
        return PX_TRUE;
    }
    if (in_size & (in_size - 1))
    {
        return PX_FALSE;
    }

    hi_pool = (px_double *)MP_Malloc(dwt->mp, (in_size * 2 + fliterlen - 1) * sizeof(px_double));
    lo_pool = (px_double *)MP_Malloc(dwt->mp, (in_size * 2 + fliterlen - 1) * sizeof(px_double));

    //up sampler
    for (i = 0; i < in_size; i++)
    {
        lo_pool[i * 2] = PX_LTIGo(&dwt->g0, in[i]);
        lo_pool[i * 2 + 1] = PX_LTIGo(&dwt->g0, 0);
    }

    for (j = 0; j < fliterlen - 1; j++)
    {
        lo_pool[i * 2 + j] = PX_LTIGo(&dwt->g0, 0);
    }

    //up sampler
    for (i = 0; i < in_size; i++)
    {
        hi_pool[i * 2] = PX_LTIGo(&dwt->g1, in[i + in_size]);
        hi_pool[i * 2 + 1] = PX_LTIGo(&dwt->g1, 0);
    }

    for (j = 0; j < fliterlen - 1; j++)
    {
        hi_pool[i * 2 + j] = PX_LTIGo(&dwt->g1, 0);
    }

    //combine
    for (i = 0; i < in_size * 2; i++)
    {
        out[i] = lo_pool[i] + hi_pool[i];
    }
    PX_memcpy(out + in_size * 2, in + in_size * 2, sizeof(px_double) * (restore_size - in_size * 2));

    MP_Free(dwt->mp, lo_pool);
    MP_Free(dwt->mp, hi_pool);

    return PX_IDWTGoEx(dwt, out, in_size * 2, out, restore_size);
}

px_bool PX_IDWTGo(PX_DWT *dwt, px_double in[], px_int in_size, px_double out[])
{
    if (!PX_IDWTGoEx(dwt, in, dwt->g0.b_count / 2, out, in_size)) { return PX_FALSE; }
    return PX_TRUE;
}

px_void PX_DWTFree(PX_DWT *dwt)
{
    PX_LTIFree(&dwt->h1);
    PX_LTIFree(&dwt->h0);
}
