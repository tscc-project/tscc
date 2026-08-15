interface Props67 { value: number; label: string }
export const View67 = (p: Props67) => (
  <section data-i="67">
    <h2>{p.label}</h2>
    {p.value > 67 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
