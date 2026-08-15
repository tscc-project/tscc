interface Props97 { value: number; label: string }
export const View97 = (p: Props97) => (
  <section data-i="97">
    <h2>{p.label}</h2>
    {p.value > 97 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
