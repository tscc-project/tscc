interface Props62 { value: number; label: string }
export const View62 = (p: Props62) => (
  <section data-i="62">
    <h2>{p.label}</h2>
    {p.value > 62 ? <strong>{p.value as number}</strong> : <span>low</span>}
  </section>
);
