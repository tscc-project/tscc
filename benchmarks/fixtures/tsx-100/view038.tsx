interface Props38 { value: number; label: string }
export const View38 = (p: Props38) => (
  <section data-i="38">
    <h2>{p.label}</h2>
    {p.value > 38 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
