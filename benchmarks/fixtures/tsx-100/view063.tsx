interface Props63 { value: number; label: string }
export const View63 = (p: Props63) => (
  <section data-i="63">
    <h2>{p.label}</h2>
    {p.value > 63 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
