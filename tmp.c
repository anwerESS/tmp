let QUERY_PFCMSGH = 'YOUR_QUERY_STRING_HERE';

let keys = QUERY_PFCMSGH.substring(QUERY_PFCMSGH.indexOf('(') + 1, QUERY_PFCMSGH.indexOf(')')).split(',');
let values = QUERY_PFCMSGH.substring(QUERY_PFCMSGH.indexOf('(') + 1, QUERY_PFCMSGH.lastIndexOf(')')).split(',');

let values2 = [];
let skip = false;
for (let i = 0; i < values.length; i++) {
    let e = values[i];
    if (e.startsWith('to_date')) {
        skip = true;
        values2.push(values[i] + ',' + values[i + 1]);
    } else if (skip) {
        skip = false;
    } else {
        values2.push(values[i]);
    }
}

let d = {};
keys.forEach((key, index) => {
    d[key] = values2[index];
});
d['IPM_ACM_MODIF'] = d['HPM_ACM_MODIF'];
delete d['IND_ARCH_INFOCENTRE'];
delete d['SENS'];
delete d['HPM_ACM_MODIF'];

let COLS_NAME = '';
let VALUES = '';
for (let [k, v] of Object.entries(d)) {
    COLS_NAME += k + ',';
    VALUES += v + ',';
}

let QUERY_PFCMSGIN = `INSERT INTO PFCMSGIN (${COLS_NAME.slice(0, -1)}) VALUES (${VALUES.slice(0, -1)});`;
console.log(QUERY_PFCMSGIN);
