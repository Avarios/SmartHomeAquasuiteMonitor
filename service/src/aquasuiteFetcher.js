// Fetch URL every 10 Seconds and check for new values

var lastUpdateDate = Date.now();

const startFetching = (fetchUrl, onFetched) => {
  setInterval(async () => {
    console.log("Fetching Data from Aquasuite");
    try {
        const fetchResult = await fetch(fetchUrl, {
            method: "GET",
          });

          const body = await fetchResult.json();
          const result = processFetchResult(body);
          let fetchTime = new Date(body.t).getTime();
          if (fetchTime > lastUpdateDate) {
            console.log(`Got Updated result with values : ${JSON.stringify(result)}`);
            lastUpdateDate = fetchTime;
            onFetched(result, undefined);
          }
    } catch (error) {
        console.error(error);
        onFetched(undefined,error);
    }

  }, 5 * 1000);
};

const processFetchResult = (body) => {
  
    return body.d.map((vals) => {
      return {
        name: vals.n,
        value: vals.v,
      };
    });
};

export { startFetching };
