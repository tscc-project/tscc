interface Props89 { value: number; label: string }
export const View89 = (p: Props89) => (
  <section data-i="89">
    <h2>{p.label}</h2>
    {p.value > 89 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
