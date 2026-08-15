interface Props29 { value: number; label: string }
export const View29 = (p: Props29) => (
  <section data-i="29">
    <h2>{p.label}</h2>
    {p.value > 29 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
