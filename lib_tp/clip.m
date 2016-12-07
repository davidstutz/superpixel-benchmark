function img_result = clip(img, minimum, maximum)

    img_result = img;
    img_result(img<minimum) = minimum;
    img_result(img>maximum) = maximum;