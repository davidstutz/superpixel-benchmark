function pvars = grab_pvars(parent, tnode, cutvar)
    pnode = parent(tnode);
    parents_this = [];
    while pnode
        parents_this = [parents_this, pnode];
        pnode = parent(pnode);
    end
    pvars = unique(cutvar(parents_this));
end