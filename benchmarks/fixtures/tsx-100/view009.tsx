interface Props9 { value: number; label: string }
export const View9 = (p: Props9) => (
  <section data-i="9">
    <h2>{p.label}</h2>
    {p.value > 9 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
