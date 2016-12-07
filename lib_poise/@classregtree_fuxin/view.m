function outfig=view(Tree,varargin)
%VIEW View classification or regression tree graphically.
%   VIEW(T) displays the decision tree T as computed by the CLASSREGTREE
%   constructor in a figure window.  Each branch in the tree is labeled
%   with its decision rule, and each terminal node is labeled with the
%   predicted value for that node.  Click on any node to get more
%   information about it.  The information displayed is specified by
%   leftmost pop-up menu at the top of the figure.
%
%   VIEW(T,'PARAM1',val1,'PARAM2',val2,...) specifies optional
%   parameter name/value pairs:
%
%      'names'       A cell array of names for the predictor variables,
%                    in the order in which they appear in the X matrix
%                    from which the tree was created (see TREEFIT)
%      'prunelevel'  Initial pruning level to display
%
%   For each branch node, the left child node corresponds to the points
%   that satisfy the condition, and the right child node corresponds to
%   the points that do not satisfy the condition.
%
%   Example:  Create and graph classification tree for Fisher's iris data.
%             The names are abbreviations for the column contents (sepal
%             length, sepal width, petal length, and petal width).
%      load fisheriris;
%      t = classregtree(meas, species);
%      view(t,'names',{'SL' 'SW' 'PL' 'PW'});
%
%   See also CLASSREGTREE, CLASSREGTREE/EVAL, CLASSREGTREE/TEST, CLASSREGTREE/PRUNE.

%   Copyright 2006-2009 The MathWorks, Inc.
%   $Revision: 1.1.6.7 $  $Date: 2009/05/07 18:32:53 $

% Process inputs
nvars = max(abs(Tree.var));
names = Tree.names;
okargs =   {'names' 'prunelevel'};
defaults = {names   0};
[eid,emsg,names,curlevel] = getargs(okargs,defaults,varargin{:});
if ~isempty(eid)
   error(sprintf('stats:treedisp:%s',eid),emsg);
end
if isempty(names)
   names = cell(nvars,1);
   for j=1:nvars
      names{j} = sprintf('x%d',j);
   end
elseif length(names)<nvars
   error('stats:treedisp:InvalidNames',...
         'Incorrect number of variable names.');
end

doclass = isequal(Tree.method,'classification');

% Create empty figure and axes to receive tree display
fig = setupfigure(doclass);
try
   adjustmenu(fig);
catch
end

% Draw tree
[X,Y,Tree] = drawtree(Tree,fig,names,curlevel);

% Save information for call-backs
fulltree = Tree;
set(fig,'ButtonDownFcn',@removelabels, ...
        'UserData',{X Y 0 names doclass fulltree curlevel});

% Update ui elements
updateenable(fig);
updatelevel(fig,curlevel,fulltree);

if nargout>0
    outfig = fig;
end

% ----------------------------------------------
function [X,Y,Tree] = drawtree(Tree,fig,names,curlevel)
%DRAWTREE Draw decision tree into specified figure.

ax = get(fig,'CurrentAxes');
splitvar = Tree.var;
cutoff = Tree.cut;
parentnode = Tree.parent;
nonroot = parentnode~=0;

% Get coordinates of nodes
isleaf = splitvar==0;
[X,Y] = layouttree(Tree,isleaf);

% Which leaves and branches are pruned?
if ~isempty(Tree.prunelist)
   isbranch = (Tree.prunelist>curlevel);
else
   isbranch = ~isleaf;
end
if any(isbranch)
   isleaf = false(size(isbranch));
   c = Tree.children(isbranch,:);
   c = c(c>0);
   isleaf(c) = 1;
   isleaf = isleaf & ~isbranch;
else
   isleaf = ~nonroot;
end
pruned = ~(isleaf | isbranch);

branchnodes = find(isbranch);
leafnodes = find(isleaf);
doclass = isequal(Tree.method,'classification');

% Get coordinates of connecting lines
p = parentnode(nonroot & ~pruned);
x = [X(nonroot & ~pruned)'; X(p)'; NaN+p'];
y = [Y(nonroot & ~pruned)'; Y(p)'; NaN+p'];

% Plot nodes and connections for unpruned nodes, but stop listening to axis
% and remember some things that may get changed during the plotting
axislistener(ax,false);
xlim = get(ax,'XLim');
ylim = get(ax,'YLim');
ud = get(ax,'UserData');
h = plot(X(branchnodes),Y(branchnodes),'b^', ...
         X(leafnodes),Y(leafnodes),'b.', ...
         x(:),y(:),'b-','Parent',ax);
set(ax,'UserData',ud,'Visible','off','XLim',xlim,'YLim',ylim);
axislistener(ax,true);

% Same for pruned nodes
t = nonroot & pruned;
p = parentnode(t);
x = [X(t)'; X(p)'; NaN+p'];
y = [Y(t)'; Y(p)'; NaN+p'];
line('Parent',ax,'XData',X(pruned),'YData',Y(pruned),'Tag','prunednode',...
     'Marker','o','Color',[.2 .2 .2],'Linestyle','none','HitTest','off');
line('Parent',ax,'XData',x(:),'YData',y(:),'Tag','prunedconnection',...
     'Marker','none','LineStyle',':','Color',[.2 .2 .2],'HitTest','off');
if length(h)==3
   set(h(1),'ButtonDownFcn',@labelpoint,'Tag','branch','MarkerSize',10);
   set(h(2),'ButtonDownFcn',@labelpoint,'Tag','leaf','MarkerSize',20);
   set(h(end),'HitTest','off','Tag','connection');
else
   set(h,'ButtonDownFcn',@labelpoint,'Tag','leaf','MarkerSize',20);
end

% Label leaf nodes with class, branch nodes with split rule
if doclass
   cnames = Tree.classname;
   ctext = cnames(Tree.class(leafnodes));
else
   ctext = num2str(Tree.class(leafnodes));
end

h = findobj(fig,'Tag','menuleaf');
vis = get(h,'Checked');
text(X(leafnodes),Y(leafnodes),ctext,'HitTest','off','Parent',ax,...
         'VerticalAlignment','top','HorizontalAlignment','center',...
          'Tag','leaflabel','Clipping','on','Visible',vis,'Interpreter','none');

lefttext = cell(length(branchnodes),1);
righttext = cell(length(branchnodes),1);
for j=1:length(branchnodes)
   k = branchnodes(j);
   cut = cutoff{k};
   if splitvar(k)>0
      varname = names{splitvar(k)};
      lefttext{j} = sprintf('%s < %g   ',varname,cut);
      righttext{j} = sprintf('  %s >= %g',varname,cut);
   else
      varname = names{-splitvar(k)};
      cats = cut{1};
      if length(cats)==1
         lefttext{j} = sprintf('%s = %s   ',varname,num2str(cats,'%g '));
      else
         lefttext{j} = sprintf('%s in (%s)   ',varname,num2str(cats,'%g '));
      end
      cats = cut{2};
      if length(cats)==1
         righttext{j} = sprintf('   %s = %s',varname,num2str(cats,'%g '));
      else
         righttext{j} = sprintf('   %s in (%s)',varname,num2str(cats,'%g '));
      end
   end
end

h = findobj(fig,'Tag','menubr');
vis = get(h,'Checked');
text(X(branchnodes),Y(branchnodes),lefttext,'HitTest','off','Parent',ax,...
     'Tag','branchlabel','Clipping','on','Visible',vis,'Interpreter','none',...
     'HorizontalAlignment','right');
text(X(branchnodes),Y(branchnodes),righttext,'HitTest','off','Parent',ax,...
     'Tag','branchlabel','Clipping','on','Visible',vis,'Interpreter','none',...
     'HorizontalAlignment','left');

% Show pruned nodes or not as desired
doprunegraph(fig);

% Adjust axes contents
dozoom(fig);

% Adjust layout of controls to fit figure
layoutfig(fig);

% ---------------------------------------------
function growprune(varargin)
%GROWPRUNE Expand or contract tree using optimal pruning sequence.

% Fetch stored information
h = gcbo;
fig = gcbf;
ud = get(fig,'UserData');
names    = ud{4};
doclass  = ud{5};
curlevel = ud{7};
fulltree = ud{6};

% Get optimal pruning sequence and current pruning level
prunelist = fulltree.prunelist;
if isequal(get(h,'Tag'),'prune')
   curlevel = min(max(prunelist),curlevel+1);
else
   curlevel = max(0,curlevel-1);
end

% Clear axes, then draw at new pruning level
ax = get(fig,'CurrentAxes');
delete(get(ax,'Children'));
[X,Y] = drawtree(fulltree,fig,names,curlevel);

% Remember everything
set(fig,'ButtonDownFcn',@removelabels, ...
        'UserData',{X Y 0 names doclass fulltree curlevel});

updateenable(fig);
updatelevel(fig,curlevel,fulltree);

% ----------------------------------------------
function updatelevel(fig,curlevel,Tree)
%UPDATELEVEL Update text display of current pruning level.

if ~isempty(Tree.prunelist)
   maxlevel = max(Tree.prunelist);
else
   maxlevel = 0;
end
h = findobj(fig,'Tag','prunelev');
set(h,'String',sprintf('%d of %d',curlevel,maxlevel));
e = get(h,'Extent');
p = get(h,'Position');
p(3) = e(3);
set(h,'Position',p);

% ----------------------------------------------
function updateenable(fig)
%UPDATEENABLE Update enabled/disabled status of buttons in figure

ud = get(fig,'UserData');
curlevel = ud{7};
fulltree = ud{6};
enableg = 'on';     % enable grow button?
enablep = 'on';     % enable prune button?
if isempty(fulltree.prunelist)
   enableg = 'off';
   enablep = 'off';
else
   maxlevel = max(fulltree.prunelist);
   if curlevel >= maxlevel
      enablep = 'off';
   end
   if curlevel <= 0;
      enableg = 'off';
   end
end
set(findobj(fig,'tag','prune'),'Enable',enablep);
set(findobj(fig,'tag','grow'),'Enable',enableg);

% ----------------------------------------------
function labelpoint(varargin)
%LABELPOINT Label point on tree in response to mouse click.

h = gcbo;
f = gcbf;
stype = get(f,'SelectionType');
if ~isequal(stype,'alt') && ~isequal(stype,'extend')
   removelabels;
end
t = get(h,'Tag');
if isequal(t,'branch') || isequal(t,'leaf')
   ud = get(f,'UserData');
   X = ud{1};       % x coordinates
   Y = ud{2};       % y coordinates
   names = ud{4};   % variable names
   doclass = ud{5}; % classification, not regression
   Tree = ud{6};    % complete tree

   if doclass
      cnames = Tree.classname;
   end
   splitvar = Tree.var;
   cutoff = Tree.cut;

   % Find closest point
   ax = get(f,'CurrentAxes');
   cp = get(ax,'CurrentPoint');
   [ignore,node] = min(abs(X-cp(1,1)) + abs(Y-cp(1,2)));

   uih = findobj(f,'Tag','clicklist');
   labeltype = get(uih,'Value');

   if isequal(labeltype,4) && doclass
      % Show fitted class probabilities
      P = Tree.classprob;
      txt = 'Class probabilities:';
      for j=1:size(P,2);
         txt = sprintf('%s\n%s = %.3g',txt,cnames{j},P(node,j));
      end

   elseif isequal(labeltype,3) && ~doclass
      % Show node statistics
      xbar = Tree.class(node);
      Nk = Tree.nodesize(node);
      if Nk > 1
         s = sqrt((Tree.nodeerr(node) * Nk) / (Nk - 1));
         txt = sprintf('N = %d\nMean = %g\nStd. dev. = %g',Nk,xbar,s);
      else
         txt = sprintf('N = %d\nMean = %g',Nk,xbar);
      end

   elseif isequal(labeltype,3) && doclass
      % Show class membership in data
      C = Tree.classcount;
      N = Tree.nodesize(node);
      txt = sprintf('Total data points = %d',N);
      for j=1:size(C,2);
         txt = sprintf('%s\n%d %s',txt,C(node,j),cnames{j});
      end

   elseif isequal(labeltype,1)
      % Get a display of the split rule at branch nodes,
      % or the majority class at leaf nodes
      if ~isequal(t,'branch')
         if doclass
            txt = sprintf('Node %d (leaf)\nClass: %s',node,cnames{Tree.class(node)});
         else
            txt = sprintf('Node %d (leaf)\nPrediction: %g',node,Tree.class(node));
         end
      elseif splitvar(node)>0
         txt = sprintf('Node %d (branch)\nRule:  %s < %g',...
                       node,names{splitvar(node)},cutoff{node});
      else
         cut = cutoff{node};
         cats = cut{1};
         if length(cats)==1
            txt = sprintf('Node %d (branch)\nRule:  %s = %s',node,...
                          names{-splitvar(node)},num2str(cats,'%g '));
         else
            txt = sprintf('Node %d (branch)\nRule:  %s in (%s)',node,...
                          names{-splitvar(node)},num2str(cats,'%g '));
         end
      end
   elseif isequal(labeltype,2)
      % Get the conditions satisfied by points in this node
      if node==1
         txt = 'Root of tree';
      else
         % Find limits for each variable along the path to this node
         nvars = max(abs(splitvar(:)));
         lims = cell(nvars,3);
         lims(:,1) = {-Inf};               % lower limit
         lims(:,2) = {Inf};                % upper limit
         lims(:,3) = num2cell((1:nvars)'); % variable number
         p = Tree.parent(node);
         c = node;
         while(p>0)
            leftright = 1 + (Tree.children(p,2)==c);
            vnum = splitvar(p);
            vcut = cutoff{p};
            if vnum>0
               if isinf(lims{vnum,3-leftright})
                  lims{vnum,3-leftright} = vcut;
               end
            else
               if ~iscell(lims{-vnum,1})
                  lims{-vnum,1} = vcut{leftright};
               end
            end
            c = p;
            p = Tree.parent(p);
         end

         % Create label listing any variable with finite limits
         txt = 'At this node:';
         for j=1:size(lims,1);
            L1 = lims{j,1};
            L2 = lims{j,2};
            if ~iscell(L1) && isinf(L1) && isinf(L2)
               continue
            end
            vnum = lims{j,3};

            if iscell(L1)
               cats = L1{1};
               if length(cats)==1
                  txt = sprintf('%s\n%s = %s',txt,names{vnum},num2str(cats,'%g '));
               else
                  txt = sprintf('%s\n%s \\in (%s)',txt,names{vnum},num2str(cats,'%g '));
               end
            elseif isinf(L1)
               txt = sprintf('%s\n%s < %g',txt,names{vnum},L2);
            elseif isinf(L2)
               txt = sprintf('%s\n%g < %s',txt,L1,names{vnum});
            else
               txt = sprintf('%s\n%g < %s < %g',txt,L1,names{vnum},L2);
            end
         end
      end
   else
      txt = '';
   end

   % Add label
   if ~isempty(txt)
      x = X(node);
      y = Y(node);
      xlim = get(gca,'xlim');
      ylim = get(gca,'ylim');
      if x<mean(xlim)
         halign = 'left';
         dx = 0.02;
      else
         halign = 'right';
         dx = -0.02;
      end
      if y<mean(ylim)
         valign = 'bottom';
         dy = 0.02;
      else
         valign = 'top';
         dy = -0.02;
      end
      h = text(x+dx*diff(xlim),y+dy*diff(ylim),txt,'Interpreter','none'); 
      yellow = [1 1 .85];
      set(h,'backgroundcolor',yellow,'margin',3,'edgecolor','k',...
            'HorizontalAlignment',halign,'VerticalAlignment',valign,...
            'tag','LinescanText','ButtonDownFcn',@startmovetips);
      line(x,y,'Color',yellow,'Marker','.','MarkerSize',20,...
           'Tag','LinescanMarker');
   end
end


% ----------------------------------------------
function startmovetips(varargin)
%STARTMOVETIPS Start movement of node tips.

f = gcbf;
set(f,'WindowButtonUpFcn',@donemovetips,...
      'WindowButtonMotionFcn',@showmovetips,...
      'Interruptible','off','BusyAction','queue');

o = gcbo;
p1 = get(f,'CurrentPoint');
a = get(f,'CurrentAxes');
ud = get(a,'UserData');
ud(1:2) = {o p1};
set(a,'UserData',ud);

% ----------------------------------------------
function showmovetips(varargin)
%SHOWMOVETIPS Show current movement of node tips.
domovetips(0,varargin{:});

% ----------------------------------------------
function donemovetips(varargin)
%DONEMOVETIPS Finish movement of node tips.
domovetips(1,varargin{:});

% ----------------------------------------------
function domovetips(alldone,varargin)
%DOMOVETIPS Carry out movement of node tips.

f = gcbf;
if alldone
   set(f,'WindowButtonUpFcn','','WindowButtonMotionFcn','');
end
a = get(f,'CurrentAxes');
ud = get(a,'UserData');
o = ud{1};
p1 = ud{2};
p2 = get(f,'CurrentPoint');
p0 = get(a,'Position');
pos = get(o,'Position');
dx = (p2(1)-p1(1)) * diff(get(a,'XLim')) / p0(3);
dy = (p2(2)-p1(2)) * diff(get(a,'YLim')) / p0(4);
pos(1) = pos(1) + dx;
pos(2) = pos(2) + dy;
set(o,'Position',pos);
ud{2} = p2;
set(a,'UserData',ud);


% ----------------------------------------------
function resize(varargin)
%RESIZE Resize figure showing decision tree.
layoutfig(gcbf)

% ----------------------------------------------
function layoutfig(f)
%LAYOUTFIG Layout figure contents

set(f,'Units','points');
fpos = get(f,'Position');

% Resize frame
h = findobj(f,'Tag','frame');
frpos = get(h,'Position');
frpos(2) = fpos(4) - frpos(4);
frpos(3) = fpos(3);
set(h,'Position',frpos);

% Resize controls inside frame
tags = {'clicktext'  'clicklist'  'magtext' 'maglist' ...
        'pruneframe' 'prunelabel' 'prunelev'};
mult = [1.6          1.35         1.6       1.35      ...
        1.7          1.6          1.6];
for j=1:length(tags)
   h = findobj(f,'Tag',tags{j});
   p = get(h,'Position');
   if j==1, theight = p(4); end
   p(2) = fpos(4) - mult(j)*theight;
   set(h,'Position',p);
end

h = findobj(f,'Tag','grow');
p = get(h,'Position');
p(2) = frpos(2)+2;
set(h,'Position',p);
h = findobj(f,'Tag','prune');
p(2) = p(2)+p(4);
set(h,'Position',p);

% Resize sliders
hh = findobj(f,'Tag','hslider');
hv = findobj(f,'Tag','vslider');
p1 = get(hh,'Position');
sw = p1(4);
p1(3) = frpos(3) - sw - 1;
set(hh,'Position',p1);
p2 = get(hv,'Position');
p2(1) = frpos(3) - sw - 1;
p2(4) = frpos(2) - sw - 1;
set(hv,'Position',p2);
if isequal(get(hh,'Visible'),'off')
   sw = 0;
end

% Resize axes
h = get(f,'CurrentAxes');
p = [0, sw, frpos(3)-sw, frpos(2)-sw];
set(h,'Position',p);


% ----------------------------------------------
function removelabels(varargin)
%REMOVELABELS Remove any labels remaining on the graph.

f = gcbf;
delete(findall(f,'Tag','LinescanMarker'));
delete(findall(f,'Tag','LinescanText'));


% ----------------------------------------------
function [X,Y] = layouttree(Tree,isleaf)
%LAYOUTTREE Select x,y coordinates of tree elements.
n = length(Tree.node);
X = zeros(n,1);
Y = X;
layoutstyle = 1;

% Plot top node on one level, its children at next level, etc.
for j=1:n
   p = Tree.parent(j);
   if p>0
      Y(j) = Y(p)+1;
   end
end
if layoutstyle==1
   % Layout style 1
   % Place terminal nodes, then put parents above them

   % First get preliminary placement, used to position leaf nodes
   for j=1:n
      p = Tree.parent(j);
      if p==0
         X(j) = 0.5;
      else
         dx = 2^-(Y(j)+1);
         if j==Tree.children(p,1)
            X(j) = X(p) - dx;
         else
            X(j) = X(p) + dx;
         end
      end
   end

   % Now make leaf nodes equally spaced, preserving their order
   leaves = find(isleaf);
   nleaves = length(leaves);
   [ignore,b] = sort(X(leaves));
   X(leaves(b)) = (1:nleaves) / (nleaves+1);

   % Position parent nodes above and between their children
   for j=max(Y):-1:0
      a = find(~isleaf & Y==j);
      c = Tree.children(a,:);
      X(a) = (X(c(:,1))+X(c(:,2)))/2;
   end
else
   % Layout style 2
   % Spread out the branch nodes, somewhat under their parent nodes
   X(Y==0) = 0.5;
   for j=1:max(Y)
      vis = (Y==j);                  % real nodes at this level
      invis = (Y==(j-1) & isleaf);   % invisible nodes to improve layout
      visrows = find(vis);
      nvis = sum(vis);
      nboth = nvis + sum(invis);
      x = [X(Tree.parent(vis))+1e-10*(1:nvis)'; X(invis)];
      [xx,xidx] = sort(x);
      xx(xidx) = 1:nboth;
      X(visrows) = (xx(1:nvis) / (nboth+1));
   end
end

k = max(Y);
Y = 1 - (Y+0.5)/(k+1);


% ---------------------------------------
function fig = setupfigure(doclass)
%SETUPFIGURE Set up uicontrols on decision tree figure.

fig = figure('IntegerHandle','off', 'NumberTitle','off', ...
             'Units','points','PaperPositionMode','auto',...
             'Tag','tree viewer');
ax = axes('Parent',fig,'UserData',cell(1,4),'XLim',0:1,'YLim',0:1);

% Set default print options
pt = printtemplate;
pt.PrintUI = 0;
set(fig,'PrintTemplate',pt)

if doclass
   figtitle = 'Classification tree viewer';
else
   figtitle = 'Regression tree viewer';
end

% Arrange figure contents
pos = [0 0 1 1];
set(ax,'Visible','off','XLim',0:1,'YLim',0:1,'Position',pos);
set(ax,'Units','points');
apos = get(ax,'Position');
fpos = get(fig,'Position');
hframe = uicontrol(fig,'Units','points','Style','frame',...
                   'Position',[0 0 1 1],'Tag','frame');

% Tip-related items
h=uicontrol(fig,'units','points','Tag','clicktext',...
            'String','Click to display:', 'style','text',...
            'HorizontalAlignment','left','FontWeight','bold');
extent = get(h,'Extent');
theight = extent(4);
aheight = apos(4);
tbottom = aheight - 1.5*theight;
posn = [2, tbottom, 150, theight];
set(h,'Position',posn);
textpos = posn;
e = get(h,'Extent');
if doclass
   choices = 'Identity|Variable ranges|Class membership|Estimated probabilities';
else
   choices = 'Identity|Variable ranges|Node statistics';
end
posn = [e(1)+e(3)+2, aheight-1.25*theight, 100, theight];
uicontrol(fig,'units','points','position',posn,'Tag','clicklist',...
            'String',choices, 'Style','pop','BackgroundColor',ones(1,3),...
            'Callback',@removelabels);
set(ax,'Position',[0 0 apos(3) tbottom]);
set(fig,'Toolbar','figure', 'Name',figtitle,'HandleVisibility','callback');

% Magnification items
textpos(1) = posn(1) + posn(3) + 10;
h=uicontrol(fig,'units','points','Tag','magtext','Position',textpos,...
            'String','Magnification:', 'style','text',...
            'HorizontalAlignment','left','FontWeight','bold');
e = get(h,'Extent');
textpos(3) = e(3);
set(h,'Position',textpos);
posn = [textpos(1)+textpos(3)+2, posn(2), 55, posn(4)];
h=uicontrol(fig,'units','points','position',posn,'Tag','maglist',...
            'String','x', 'Style','pop','BackgroundColor',ones(1,3),...
            'Callback',@domagnif);
adjustcustomzoom(h,false);

% Prune-related items
textpos(1) = posn(1) + posn(3) + 10;
h = uicontrol(fig,'units','points','position',textpos,'Tag','prunelabel',...
              'Style','text','HorizontalAlignment','left',...
              'FontWeight','bold','String','Pruning level:');
e = get(h,'Extent');
textpos(3) = e(3);
set(h,'Position',textpos);

posn(1) = textpos(1) + textpos(3) + 5;
posn(2) = posn(2) - 0.25*e(4);
posn(3) = 60;
posn(4) = 1.5*e(4);
textpos(1) = posn(1) + 3;
textpos(3) = posn(3) - 6;
uicontrol(fig,'Style','frame','Units','points','Position',posn,...
              'Tag','pruneframe');
uicontrol(fig,'units','points','position',textpos,'Tag','prunelev',...
              'Style','text','HorizontalAlignment','left',...
              'FontWeight','bold','String','1234 of 9999');

% Create an arrow for labeling button controls
fcolor = get(fig,'Color');
ar = ...
[1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 0 0 0 0 0 0 0 0 0 0 0 1
 1 1 0 0 0 0 0 0 0 0 0 1 1
 1 1 1 0 0 0 0 0 0 0 1 1 1
 1 1 1 1 0 0 0 0 0 1 1 1 1
 1 1 1 1 1 0 0 0 1 1 1 1 1
 1 1 1 1 1 1 0 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1
 1 1 1 1 1 1 1 1 1 1 1 1 1];
ar = repmat(ar,[1 1 3]);
ar(:,:,1) = min(ar(:,:,1),fcolor(1));
ar(:,:,2) = min(ar(:,:,2),fcolor(2));
ar(:,:,3) = min(ar(:,:,3),fcolor(3));

posn(1) = posn(1) + posn(3);
posn(4) = posn(4)/2;
posn(3) = posn(4);
uicontrol(fig,'units','points','position',posn,'Tag','prune',...
              'CData',ar(end:-1:1,:,:),...
              'Style','pushbutton','Callback',@growprune);
posn(2) = posn(2) + posn(4);
uicontrol(fig,'units','points','position',posn,'Tag','grow',...
              'CData',ar,...
              'Style','pushbutton','Callback',@growprune);

if posn(1)+posn(3) > fpos(3)
   fpos(3) = posn(1)+posn(3);
   set(fig,'Position',fpos);
   apos(3) = fpos(3);
   set(ax,'Position',apos);
end

% Adjust frame position
lowest = min(posn(2),textpos(2))-2;
frpos = apos;
frpos(4) = 1.1*(apos(4)-lowest);
frpos(2) = apos(4) - frpos(4);
set(hframe,'Position',frpos);

% Add scroll bars, start out invisible
h1 = uicontrol(fig,'Style','slider','Tag','hslider','Visible','off',...
               'Units','points','Callback',@dopan);
p1 = get(h1,'Position');
sw = p1(4);               % default slider width
p1(1:2) = 1;
p1(3) = fpos(3)-sw;
set(h1,'Position',p1);
p2 = [fpos(3)-sw, sw, sw, frpos(2)-sw];
uicontrol(fig,'Style','slider','Tag','vslider','Visible','off',...
               'Units','points','Position',p2,'Callback',@dopan);

% Add new menu before the Window menu
hw = findall(fig,'Type','uimenu', 'Label','&Window');
h0 = uimenu(fig,'Label','T&ree','Position',get(hw,'Position'));
uimenu(h0, 'Label','Show &Full Tree', 'Position',1,'Tag','menufull',...
           'Checked','on','Callback',@domenu);
uimenu(h0, 'Label','Show &Unpruned Nodes', 'Position',2,'Tag','menuunpr',...
           'Checked','off','Callback',@domenu);
uimenu(h0, 'Label','Label &Branch Nodes', 'Position',3,'Tag','menubr',...
           'Checked','on','Callback',@domenu,'Separator','on');
uimenu(h0, 'Label','Label &Leaf Nodes', 'Position',4,'Tag','menuleaf',...
           'Checked','on','Callback',@domenu);

set(fig,'ResizeFcn',@resize);

% ------------------------------------------
function domenu(varargin)
%DOMENU Carry out menu actions for tree viewer.

o = gcbo;
f = gcbf;
t = get(o,'Tag');
switch(t)
 % Change display from full tree to unpruned nodes or vice versa
 case {'menufull' 'menuunpr'}
   ischecked = isequal(get(o,'Checked'),'on');
   isfull = isequal(t,'menufull');
   if isfull
      dofull = ~ischecked;
   else
      dofull = ischecked;
   end
   mfull = findobj(f,'Type','uimenu','Tag','menufull');
   munpr = findobj(f,'Type','uimenu','Tag','menuunpr');
   if dofull
      set(mfull,'Checked','on');
      set(munpr,'Checked','off');
   else
      set(mfull,'Checked','off');
      set(munpr,'Checked','on');
   end
   doprunegraph(f,dofull);
   dozoom(f);

 % Turn on/off branch labels
 case 'menubr'
   curval = get(o,'Checked');
   if isequal(curval,'on')
      set(o,'Checked','off');
      h = findobj(f,'Type','text','Tag','branchlabel');
      set(h,'Visible','off');
   else
      set(o,'Checked','on');
      h = findobj(f,'Type','text','Tag','branchlabel');
      set(h,'Visible','on');
   end

 % Turn on/off leaf labels
 case 'menuleaf'
   curval = get(o,'Checked');
   if isequal(curval,'on')
      set(o,'Checked','off');
      h = findobj(f,'Type','text','Tag','leaflabel');
      set(h,'Visible','off');
   else
      set(o,'Checked','on');
      h = findobj(f,'Type','text','Tag','leaflabel');
      set(h,'Visible','on');
   end
end


% ------------------------------------------
function doprunegraph(f,dofull)
%DOPRUNEGRAPH Adjust graph to show full/pruned setting

a = get(f,'CurrentAxes');
h1 = findobj(a,'Type','line','Tag','prunednode');
h2 = findobj(a,'Type','line','Tag','prunedconnection');

% Figure out whether to show full tree
if nargin<2
   o = findobj(f,'Type','uimenu','Tag','menufull');
   dofull = isequal(get(o,'Checked'),'on');
end

% Adjust graph
if dofull
   set(h1,'Visible','on');
   set(h2,'Visible','on');
   xlim = get(a,'XLim');
   ylim = get(a,'YLim');
   bigxlim = 0:1;
   bigylim = 0:1;
else
   set(h1,'Visible','off');
   set(h2,'Visible','off');
   h1 = findobj(f,'Type','line','Tag','leaf');
   h2 = findobj(f,'Type','line','Tag','branch');
   x1 = get(h1,'XData');
   y1 = get(h1,'YData');
   y2 = get(h2,'YData');
   dx = 1 / (1+length(x1));
   ally = sort(unique([y1(:); y2(:)]));
   if length(ally)>1
      dy = 0.5 * (ally(2)-ally(1));
   else
      dy = 1-ally;
   end
   xlim = [min(x1)-dx, max(x1)+dx];
   ylim = [min(ally)-dy, max(ally)+dy];
   bigxlim = 0:1;
   bigylim = [ylim(1) 1];
end
axislistener(a,false);
set(a,'XLim',xlim,'YLim',ylim);
axislistener(a,true);
hh = findobj(f,'Tag','hslider');
set(hh,'UserData',bigxlim);
hv = findobj(f,'Tag','vslider');
set(hv,'UserData',bigylim);


% ------------------------------------------
function domagnif(varargin)
%DOMAGNIF React to magnification level change

f = gcbf;
o = gcbo;

% We need sliders if the magnification level is > 100%
h = [findobj(f,'Tag','hslider'), findobj(f,'Tag','vslider')];
maglevel = get(o,'Value');
if maglevel==1
   set(h,'Visible','off');
else
   set(h,'Visible','on');
end

% Adjust layout if necessary
resize;

% Adjust axes contents
dozoom(f);

% Remove custom zoom amount from list if not in use
if maglevel<=4
   adjustcustomzoom(o,false);
end

% Turn off any manual zooming
zoom(f,'off');

% --------------------------------------------------
function adjustcustomzoom(o,add)
%ADJUSTCUSTOMZOOM Add or remove special custom magnification level
nchoices = size(get(o,'String'),1);
choices = '100%|200%|400%|800%';
if ~add && nchoices~=4
   set(o,'String',choices);
elseif add && nchoices~=5
   choices = [choices '|Custom'];
   set(o,'String',choices);
end

% ------------------------------------------
function dozoom(f)
%DOZOOM Adjust axes contents to match magnification settings

a = get(f,'CurrentAxes');
hh = findobj(f,'Tag','hslider');
hv = findobj(f,'Tag','vslider');
hm = findobj(f,'Tag','maglist');

% Get information about x/y ranges and current midpoint
bigxlim = get(hh,'UserData');
bigylim = get(hv,'UserData');
xlim = get(a,'XLim');
ylim = get(a,'YLim');
currx = (xlim(1)+xlim(2))/2;
curry = (ylim(1)+ylim(2))/2;

% How much are we magnifying each axis?
magfact = [1 2 4 8];
mag = get(hm,'Value');
if mag<=4
   magfact = magfact(mag)*ones(1,2);
else
   magfact = [diff(bigxlim)/diff(xlim), diff(bigylim)/diff(ylim)];
end
magfact = max(magfact,1);

if all(magfact==1)                 % no magnification
   xlim = bigxlim;
   ylim = bigylim;
else                               % magnify by showing a subset of range
   magfact = max(magfact,1.01);
   dx = diff(bigxlim)/magfact(1);
   dy = diff(bigylim)/magfact(2);
   xval = max(bigxlim(1), min(bigxlim(2)-dx, currx-dx/2));
   xlim = xval + [0,dx];
   yval = max(bigylim(1), min(bigylim(2)-dy, curry-dy/2));
   ylim = yval + [0,dy];
   set(hh,'Min',bigxlim(1),'Max',bigxlim(2)-dx,'Value',xval);
   set(hv,'Min',bigylim(1),'Max',bigylim(2)-dy,'Value',yval);
end
axislistener(a,false);
set(a,'XLim',xlim,'YLim',ylim);
axislistener(a,true);


% ------------------------------------------
function dopan(varargin)
%DOPAN Pan around magnified tree display

f = gcbf;
a = get(f,'CurrentAxes');
o = gcbo;
val = get(o,'Value');

axislistener(a,false);
if isequal(get(o,'Tag'),'hslider')
   xlim = get(a,'XLim');
   xlim = xlim + (val-xlim(1));
   set(a,'XLim',xlim);
else
   ylim = get(a,'YLim');
   ylim = ylim + (val-ylim(1));
   set(a,'YLim',ylim);
end
axislistener(a,true);


% -----------------------------------------
function axislistener(a,enable)
%AXISLISTENER Enable or disable listening to axis limit changes

f = get(a,'Parent');
ud = get(a,'UserData');
if enable
   % Start listening to axis limit changes
   list1 = addlistener(a, 'XLim', 'PostSet', @(src,evt) customzoom(f));
   list2 = addlistener(a, 'YLim', 'PostSet', @(src,evt) customzoom(f));
   ud(3:4) = {list1 list2};
else
   % Delete listeners
   for j=3:4
      lstnr = ud{j};
      if ~isempty(lstnr) && ishandle(lstnr), delete(lstnr); end
   end
   ud(3:4) = {[]};
end
set(a,'UserData',ud);


% -----------------------------------------
function customzoom(f)
%CUSTOMPAN Deal with panning of a zoomed tree view

a = get(f,'CurrentAxes');
xlim = get(a,'XLim');
ylim = get(a,'YLim');

hh = findobj(f,'Tag','hslider');
hv = findobj(f,'Tag','vslider');
hm = findobj(f,'Tag','maglist');

bigxlim = get(hh,'UserData');
bigylim = get(hv,'UserData');
magfact = [1 2 4 8];

% Figure out if we have a standard zoom amount (100%, 200%, etc.) or
% a custom zoom amount
xratio = diff(bigxlim) / diff(xlim);
yratio = diff(bigylim) / diff(ylim);
standard = abs(xratio-yratio)<=0.02 & abs(xratio-round(xratio))<=0.02 ...
                                    & abs(yratio-round(yratio))<=0.02;
if standard
   xratio = round(xratio);
   standard = ismember(xratio,magfact);
end

% Update the magnification setting
if standard
   set(hm,'Value',find(magfact==xratio));
   adjustcustomzoom(hm,false);
   if xratio==1
      h = [findobj(f,'Tag','hslider'), findobj(f,'Tag','vslider')];
      set(h,'Visible','off');
   end
else
   adjustcustomzoom(hm,true);
   set(hm,'Value',5);
   h = [findobj(f,'Tag','hslider'), findobj(f,'Tag','vslider')];
   set(h,'Visible','on');
end

dozoom(f);

% ---------------------- helper to fix menu contents
function adjustmenu(fig)
%ADJUSTMENU Adjust contents of curve fit plot menus and toolbar

% Remove some menus entirely]
badmenus = {'&Edit' '&View' '&Insert'};
h = findall(fig, 'Type','uimenu', 'Parent',fig);
for j=1:length(badmenus)
   h0 = findall(h,'flat', 'Label',badmenus{j});
   if (~isempty(h0))
      j = find(h==h0);
      delete(h0);
      h(j) = [];
   end
end

% Add or remove some items from other menus
% Fix FILE menu
h0 = findall(h,'flat', 'Label','&File');
h1 = findall(h0, 'Type','uimenu', 'Parent',h0);
for j=length(h1):-1:1
   mlabel = get(h1(j),'Label');
   if isempty(findstr(mlabel,'Print...'))
      delete(h1(j));
      h1(j) = [];
   end
end

% Fix TOOLS menu
h0 = findall(h,'flat', 'Label','&Tools');
h1 = findall(h0, 'Type','uimenu', 'Parent',h0);
for j=length(h1):-1:1
   mlabel = get(h1(j),'Label');
   if (isempty(findstr(mlabel,'Zoom'))&& isempty(findstr(mlabel,'Pan')))
     delete(h1(j));
     h1(j) = [];
   end
end

% Fix HELP menu
h0 = findall(h,'flat', 'Label','&Help');
h1 = findall(h0, 'Type','uimenu', 'Parent',h0);
delete(h1);
uimenu(h0, 'Label', '&Help Tree Viewer', 'Position',1,'Callback',@helpviewer);

% Remove icons that don't apply here.  Keep zoom, PAN and print only.
h0 = findall(fig,'Type','uitoolbar');
h1 = findall(h0,'Parent',h0);
for j=length(h1):-1:1
   mlabel = get(h1(j),xlate('TooltipString'));
   if isempty(findstr(mlabel,'Zoom')) && isempty(findstr(mlabel,'Print')) && isempty(findstr(mlabel,'Pan')) 
      delete(h1(j));
   end
end

% ---------------------- callback to display help
function helpviewer(varargin)
mapfilename = [docroot '/toolbox/stats/stats.map'];
helpview(mapfilename, 'view_classregtree_ref');

