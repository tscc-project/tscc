interface Props94 { value: number; label: string }
export const View94 = (p: Props94) => (
  <section data-i="94">
    <h2>{p.label}</h2>
    {p.value > 94 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
