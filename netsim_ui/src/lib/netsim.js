
import { writable } from "svelte/store";

const STATUS_URI="http://localhost:8080/status";
const PROFILES_URI="http://localhost:8080/profiles";

export const status = writable({
    enabled: false,
    profile: {}
});
export const profiles = writable({});
export const defaultProfile = {
    ecn: false,
    loss_model: "random",
    delay_unit_ui: {
        value: "ms",
        label: "ms"
    },
    delay_unit: "ms",
    bandwidth_unit_ui: {
        value: "Mbps",
        label: "Mbit/s"
    },
    bandwidth_unit: "Mbps",
};

function addDefaultToStatus(status)
{
    status.profile = {
        ...defaultProfile,
        ...status.profile 
    };
}

export async function getStatus() {
    const res = await fetch(STATUS_URI);

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let status =  await res.json();
    addDefaultToStatus(status);
    return status;
}

export async function saveStatus(status) 
{
    let profile = status.profile;
    if (profile.loss) {
        profile.loss = Number(profile.loss);
    }
    if (profile.delay) {
        profile.delay = Number(profile.delay);
    }
    if (profile.bandwidth) {
        profile.bandwidth = Number(profile.bandwidth);
    }
    profile.delay_unit = profile.delay_unit_ui.value;
    profile.bandwidth_unit = profile.bandwidth_unit_ui.value;
    console.log(status);

    const res = await fetch(STATUS_URI, {
        method: "POST",
        body: JSON.stringify(status)
    });

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let status1 =  await res.json();
    addDefaultToStatus(status1);
    return status1;
}

export async function getProfiles()
{
    const res = await fetch(PROFILES_URI);

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let profiles =  await res.json();
    return profiles;
}

export async function saveProfiles(profiles) 
{
    console.log("Profiles: ", profiles);

    const res = await fetch(PROFILES_URI, {
        method: "POST",
        body: JSON.stringify(profiles)
    });

    if (!res.ok) {
        throw new Error('Request failed');
    }

    let profiles1 =  await res.json();
    return profiles1;
}