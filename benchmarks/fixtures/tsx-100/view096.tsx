interface Props96 { value: number; label: string }
export const View96 = (p: Props96) => (
  <section data-i="96">
    <h2>{p.label}</h2>
    {p.value > 96 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
