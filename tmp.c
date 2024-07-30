HEADER = """
*********************************************
* Nom du fichier: pa_ng_UNITYXXXXXXXXXX_009_rcy.sql
* Generé le: 09/02/2024
* Développeur: 
*********************************************
"""
FOOTER = """END;
/
"""

QUERY_ROM = """
INSERT INTO RECYCLAGE_MQ 
(REC_ID, ENR_CODE, MATCHING_REFERENCE, ORIGINAL_FLOW, NB_TRIES)
VALUES ({}, '{}', '{}', vORIGINAL_FLOW, '{}');
"""

content = HEADER

content += `
DECLARE
vORIGINAL_FLOW CLOB;
BEGIN
`

function textToLines(txt) {
    return txt.split('\n');
}

function splitFlux(flux, charNbr = MAX_LINE_LGTH) {
    let splittedFluxList = [];
    for (let i = 0; i < flux.length; i += charNbr) {
        splittedFluxList.push(flux.substring(i, i + charNbr));
    }
    let formattedFlux = '\n'.join(splittedFluxList);
    return `    ${formattedFlux}    `;
}

let f = `*INSTRINST      MTS49      LUX          REIM1000034221597`;

console.log(splitFlux(f));

let lines = textToLines(f);
let INITIAL_ID = 1;

for (let line of lines) {
    content += QUERY_ROM.replace('{}', INITIAL_ID++)
                        .replace('{}', splitFlux(line))
                        .replace('{}', 'INITIAL_ID_ENR_CODE')
                        .replace('{}', 'MATCHING_REFERENCE')
                        .replace('{}', 'NB_TRIES');
}

content += FOOTER;
