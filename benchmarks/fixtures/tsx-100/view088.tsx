interface Props88 { value: number; label: string }
export const View88 = (p: Props88) => (
  <section data-i="88">
    <h2>{p.label}</h2>
    {p.value > 88 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
