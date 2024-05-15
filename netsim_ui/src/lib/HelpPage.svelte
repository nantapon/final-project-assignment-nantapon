<script>
    import * as Card from "$lib/components/ui/card/index";
    import * as Accordion from "$lib/components/ui/accordion/index";
    import { mode } from "mode-watcher";
    import { config } from "./netsim";
</script>

<Card.Root>
    <Card.Header>
        <Card.Title>Help</Card.Title>
    </Card.Header>
    <Card.Content class="space-y-2">
        <Accordion.Root class="w-[350px]" value={["setup"]} multiple>
            <Accordion.Item value="setup">
                <Accordion.Trigger>Set up</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            The machine under test must be set up to route the
                            message to the network simulator.
                        </p>
                        <p>
                            The example below show how to set up default route
                            for both ipv4 and ipv6. For IPv6, use -6 and global
                            ip address for network-simulator (not starting with
                            fe80::)
                        </p>
                        <pre>
sudo ip route add default via {$config.addr} metric 10                             
sudo ip route -6 add default via {$config.addr6} metric 10
                        </pre>
                        <p>You can also set route for more limited targets. This is a must if you want to use in LAN.</p>
                        <pre>
sudo ip route add &lt;destination_network&gt; via {$config.addr} dev &lt;device&gt; metric 10
sudo ip route -6 add &lt;destination_network&gt; via {$config.addr6} dev &lt;device&gt; metric 10
</pre>
<p>The example below is for windows in command path with administrative privileges</p>
<pre>
netsh interface ipv4 add route &lt;destination_network&gt; &lt;interface_index&gt; {$config.addr} metric=10
netsh interface ipv6 add route &lt;destination_network&gt; &lt;interface_index&gt; {$config.addr6} metric=10
</pre>
<p>For Mac:</p>
<pre>
sudo route -n add &lt;destination_network&gt; {$config.addr}
sudo route -n add -inet6 &lt;destination_network&gt; {$config.addr6} 
</pre>
</article>
                </Accordion.Content>
            </Accordion.Item>

            <Accordion.Item value="limitation">
                <Accordion.Trigger>Current Limitations</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <ul>
                            <li>The bandwidth is shared by all connections.</li>
                            <li>Only one profile can be active at a time.</li>
                            <li>We cannot configure network address in the app. We have to rely on DHCP assignment to get permanent IP address.</li>
                        </ul>
                    </article>
                </Accordion.Content>
            </Accordion.Item>

            <Accordion.Item value="state loss model">
                <Accordion.Trigger>State loss model</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            (From tc-netem manpage) adds packet losses according
                            to the 4-state Markov using the transition
                            probabilities as input parameters. The parameter p13
                            is mandatory and if used alone corresponds to the
                            Bernoulli model. The optional parameters allows to
                            extend the model to 2-state (p31), 3-state (p23 and
                            p32) and 4-state (p14). State 1 corresponds to good
                            reception, State 4 to independent losses, State 3 to
                            burst losses and State 2 to good reception within a
                            burst.
                        </p>
                        <img
                            src={$mode === "dark"
                                ? "state_loss_dark.png"
                                : "state_loss.png"}
                            alt="state loss model"
                        />
                    </article>
                </Accordion.Content>
            </Accordion.Item>

            <Accordion.Item value="about">
                <Accordion.Trigger>About Network Simulator</Accordion.Trigger>
                <Accordion.Content>
                    <article class="prose text-muted-foreground">
                        <p>
                            This project is submitted as a final project for
                            coursera course 'Linux Embedded System Topics and
                            Projects' offered by UC Boulder.
                        </p>
                    </article>
                </Accordion.Content>
            </Accordion.Item>
        </Accordion.Root>
    </Card.Content>
</Card.Root>
