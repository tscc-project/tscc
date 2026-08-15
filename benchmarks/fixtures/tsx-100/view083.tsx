interface Props83 { value: number; label: string }
export const View83 = (p: Props83) => (
  <section data-i="83">
    <h2>{p.label}</h2>
    {p.value > 83 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
