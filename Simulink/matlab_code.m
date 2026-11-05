function D = mppt_po(V, I)
    persistent V_old P_old D_old count;
    
    if isempty(V_old)
        V_old = 36;
        P_old = 0;
        D_old = 0.5; % %50 doluluk ile başla
        count = 0;
    end
    
    % MPPT'yi her 100 simülasyon adımında bir (1 ms) güncelle
    count = count + 1;
    if count < 100
        D = D_old;
        return;
    end
    count = 0;
    
    P = V * I;
    delta_P = P - P_old;
    delta_V = V - V_old;
    
    delta_D = 0.001; % Kararlı küçük adım
    
    % Buck dönüştürücü için doğru eğim takibi:
    if delta_P > 0
        if delta_V > 0
            D = D_old - delta_D;
        else
            D = D_old + delta_D;
        end
    elseif delta_P < 0
        if delta_V > 0
            D = D_old + delta_D;
        else
            D = D_old - delta_D;
        end
    else
        D = D_old;
    end
    
    % Sınırlandırma (Geniş aralık)
    if D > 0.90
        D = 0.90;
    elseif D < 0.10
        D = 0.10;
    end
    
    V_old = V;
    P_old = P;
    D_old = D;
end