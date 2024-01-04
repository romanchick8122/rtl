docker run `
    --rm `
    --volume="${PWD}:/srv/jekyll" `
    --volume="${PWD}/.cache:/usr/local/bundle" `
    --publish-all `
    jekyll/jekyll `
    jekyll serve --watch