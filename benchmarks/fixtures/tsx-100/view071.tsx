interface Props71 { value: number; label: string }
export const View71 = (p: Props71) => (
  <section data-i="71">
    <h2>{p.label}</h2>
    {p.value > 71 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
