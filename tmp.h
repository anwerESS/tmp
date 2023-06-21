SELECT
    DRI.DRI_CODE                AS " Référence SIB",
    DRI.SAC_CODE                AS "Numéro de compte client",
    SCO.SCO_CODE                AS "Code Isin",
    DRI.DRI_QUANTITY       AS "Quantitet"
FROM
    (SELECT DRI_CODE, SAC_CODE, DRI_QUANTITY, SEC_CODE
        FROM
            DRI_HISTO
        WHERE     dri_create_date BETWEEN TO_DATE('01/01/2023', 'DD/MM/YYYY') AND TO_DATE('01/06/2023', 'DD/MM/YYYY')

    ) DRI


     INNER JOIN
    ( SELECT
        SEC_CODE,
        SCO_CODE,
        COF_CODE
        FROM SECURITY_CODIF
    ) SCO ON DRI.SEC_CODE = SCO.SEC_CODE AND SCO.COF_CODE = 'ISIN';
