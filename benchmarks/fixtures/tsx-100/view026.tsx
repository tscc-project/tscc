interface Props26 { value: number; label: string }
export const View26 = (p: Props26) => (
  <section data-i="26">
    <h2>{p.label}</h2>
    {p.value > 26 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
