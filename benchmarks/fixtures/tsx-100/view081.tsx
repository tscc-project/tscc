interface Props81 { value: number; label: string }
export const View81 = (p: Props81) => (
  <section data-i="81">
    <h2>{p.label}</h2>
    {p.value > 81 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
